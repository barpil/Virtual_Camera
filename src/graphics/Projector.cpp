//
// Created by Bcom_ on 04.04.2026.
//

#include <SFML/System/Vector2.hpp>

#include "Projector.h"

#include <cmath>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "../utils/3DSpaceUtils.h"
#include "binaries/RobotoMonoFont.h"
#include "elements/Camera.h"

const double ARCBALL_SENSITIVITY = 1;

Projector::Projector(const int screenWidth, const int screenHeight, const Camera &camera)
    : screenWidth(screenWidth), screenHeight(screenHeight), cameraPositionText(font), camera(camera) {
    window.create(sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}),
                  "Virutal Camera");
    window.setFramerateLimit(60);

    lines.setPrimitiveType(sf::PrimitiveType::Lines);

    if (!font.openFromMemory(RobotoMonoFont_ttf, RobotoMonoFont_ttf_len)) {
        std::cerr << "Failed to load font\n";
    }
    cameraPositionText.setCharacterSize(16);
}


void Projector::drawFigure(const Figure &figure) {
    for (const auto &[start, end]: figure.edges) {
        const sf::Vector2f p1 = projectPoint(figure.points[start]);
        const sf::Vector2f p2 = projectPoint(figure.points[end]);

        lines.append(sf::Vertex{p1, sf::Color::Green});
        lines.append(sf::Vertex{p2, sf::Color::Green});
    }
}

void Projector::refreshDisplay() {
    window.clear();
    refreshOnScreenText();
    redrawFigures();
    window.display();
}

void Projector::refreshOnScreenText() {
    cameraPositionText.setString(std::format(
        "Camera:\nPosition: (x:{:.2f}; y:{:.2f}; z:{:.2f})\nRotation: (ax:{:.2f}; ay:{:.2f}; az:{:.2f})",
        camera.getCameraPosition().x,
        camera.getCameraPosition().y, camera.getCameraPosition().z,
        //Przeliczenie pozycji osi kamery na rotacje w stopniach
        std::asin(-camera.getLocalOrientation().forward.y) * 180.0 / M_PI,
        std::atan2(camera.getLocalOrientation().forward.x, camera.getLocalOrientation().forward.z) * 180.0 / M_PI,
        std::atan2(camera.getLocalOrientation().right.y, camera.getLocalOrientation().up.y) * 180.0 / M_PI));
    cameraPositionText.setPosition({
        static_cast<float>(window.getSize().x) - cameraPositionText.getLocalBounds().size.x - 10.f,
        10.f
    });
    window.draw(cameraPositionText);
}

void Projector::clearLineBuffer() {
    lines.clear();
}

void Projector::redrawFigures() {
    clearLineBuffer();
    for (Figure const &figure: figures) {
        drawFigure(figure);
    }
    window.draw(lines);
}

void Projector::render(const std::vector<Figure> &figureList) {
    this->figures = figureList;
    refreshDisplay();
    std::optional<sf::Vector2i> dragStart;
    bool isDragging = false;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            else if (event->is<sf::Event::KeyPressed>()) {
                const auto &key = event->getIf<sf::Event::KeyPressed>();
                onKeyPressed(key->code);
                refreshDisplay();
            } else if (event->is<sf::Event::MouseWheelScrolled>()) {
                onMouseWheelScrolled(*event->getIf<sf::Event::MouseWheelScrolled>());
                refreshDisplay();
            } else if (event->is<sf::Event::MouseButtonPressed>()) {
                if (const auto &mouse = event->getIf<sf::Event::MouseButtonPressed>();
                    mouse->button == sf::Mouse::Button::Left) {
                    dragStart = sf::Mouse::getPosition(window);
                    isDragging = true;
                }
            } else if (event->is<sf::Event::MouseButtonReleased>()) {
                if (const auto &mouse = event->getIf<sf::Event::MouseButtonReleased>();
                    mouse->button == sf::Mouse::Button::Left) {
                    isDragging = false;
                }
            } else if (event->is<sf::Event::Resized>()) {
                const auto &resized = event->getIf<sf::Event::Resized>();
                window.setView(sf::View(sf::FloatRect(
                    {0.f, 0.f},
                    {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)}
                )));
                refreshDisplay();
            }
        }

        if (isDragging) {
            sf::Vector2i dragEnd = sf::Mouse::getPosition(window);
            if (dragStart) {
                onDrag({static_cast<double>(dragStart->x), static_cast<double>(dragStart->y)}, {
                           static_cast<double>(dragEnd.x), static_cast<double>(dragEnd.y)
                       });
                refreshDisplay();
                dragStart = dragEnd;
            }
        }
    }
}


sf::Vector2f Projector::projectPoint(const Point3D &p) const {
    Point2D point2D = utils::project3DTo2D(camera.getCameraPosition(), camera.getLocalOrientation(), camera.getFocal(), p);
    //Zeby zgadzala sie skala po powiekszeniu okna
    const double scaleX = static_cast<double>(window.getSize().x) / screenWidth;
    const double scaleY = static_cast<double>(window.getSize().y) / screenHeight;
    const double scale = std::min(scaleX, scaleY);


    const double x2d = point2D.x * scale
                       + static_cast<double>(window.getSize().x) / 2;

    const double y2d = -point2D.y * scale
                       + static_cast<double>(window.getSize().y) / 2;

    return {static_cast<float>(x2d), static_cast<float>(y2d)};
}

void Projector::onKeyPressed(const sf::Keyboard::Key key) {
    switch (key) {
        case sf::Keyboard::Key::W:
            camera.move(0, 0, 0.1);
            break;
        case sf::Keyboard::Key::A:
            camera.move(-0.1, 0, 0);
            break;
        case sf::Keyboard::Key::S:
            camera.move(0, 0, -0.1);
            break;
        case sf::Keyboard::Key::D:
            camera.move(0.1, 0, 0);
            break;
        case sf::Keyboard::Key::Space:
            camera.move(0, 0.1, 0);
            break;
        case sf::Keyboard::Key::LShift:
            camera.move(0, -0.1, 0);
            break;
        case sf::Keyboard::Key::R:
            camera.levelLocalHorizon();
            break;
        case sf::Keyboard::Key::Up: {
            const Point2D screenCenter = {
                static_cast<double>(window.getSize().x) / 2, static_cast<double>(window.getSize().y) / 2
            };
            auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
                screenCenter, {screenCenter.x, screenCenter.y + 10}, window.getSize().x,
                window.getSize().y, ARCBALL_SENSITIVITY);
            camera.rotate(xRotation, yRotation, 0);
            break;
        }
        case sf::Keyboard::Key::Down: {
            const Point2D screenCenter = {
                static_cast<double>(window.getSize().x) / 2, static_cast<double>(window.getSize().y) / 2
            };
            auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
                screenCenter, {screenCenter.x, screenCenter.y - 10}, window.getSize().x,
                window.getSize().y, ARCBALL_SENSITIVITY);
            camera.rotate(xRotation, yRotation, 0);
            break;
        }
        case sf::Keyboard::Key::Left: {
            const Point2D screenCenter = {
                static_cast<double>(window.getSize().x) / 2, static_cast<double>(window.getSize().y) / 2
            };
            auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
                screenCenter, {screenCenter.x - 10, screenCenter.y}, window.getSize().x,
                window.getSize().y, ARCBALL_SENSITIVITY);
            camera.rotate(xRotation, yRotation, 0);
            break;
        }
        case sf::Keyboard::Key::Right: {
            const Point2D screenCenter = {
                static_cast<double>(window.getSize().x) / 2, static_cast<double>(window.getSize().y) / 2
            };
            auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
                screenCenter, {screenCenter.x + 10, screenCenter.y}, window.getSize().x,
                window.getSize().y, ARCBALL_SENSITIVITY);
            camera.rotate(xRotation, yRotation, 0);
            break;
        }
        case sf::Keyboard::Key::Q:
            camera.rotate(0, 0, 1);
            break;
        case sf::Keyboard::Key::E:
            camera.rotate(0, 0, -1);
            break;
        default:
            break;
    }
}


void Projector::onMouseWheelScrolled(const sf::Event::MouseWheelScrolled &event) {
    camera.zoom(event.delta * 5);
}

void Projector::onDrag(Point2D dragStart, Point2D dragEnd) {
    auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
        dragStart, dragEnd, window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
    camera.rotate(xRotation, yRotation, 0); //Rotacja Z ustawiana na 0, bo inaczej zbyt ciezko sterowac
}
