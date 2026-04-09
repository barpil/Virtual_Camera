//
// Created by Bcom_ on 04.04.2026.
//

#include <SFML/System/Vector2.hpp>

#include "Projector.h"

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "../utils/3DSpaceUtils.h"
#include "elements/Camera.h"


Projector::Projector(const int screenWidth, const int screenHeight, const Camera &camera)
    : screenWidth(screenWidth), screenHeight(screenHeight), cameraPositionText(font), camera(camera) {
    window.create(sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}),
                  "Virutal Camera");
    window.setFramerateLimit(60);

    lines.setPrimitiveType(sf::PrimitiveType::Lines);

    //Ustawienie napisu z pozycja kamery
    if (!font.openFromFile("../resources/arial.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    cameraPositionText.setCharacterSize(16);

}


void Projector::drawFigure(const Figure &figure) {
    for (const auto &[start, end]: figure.edges) {
        const sf::Vector2f p1 = pointToVector2f(figure.points[start]);
        const sf::Vector2f p2 = pointToVector2f(figure.points[end]);

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
    cameraPositionText.setString(std::format("Camera (x:{:.2f}; y:{:.2f}; z:{:.2f})", camera.getCameraPosition().x,
                                             camera.getCameraPosition().y, camera.getCameraPosition().z));
    cameraPositionText.setPosition({
        static_cast<float>(window.getSize().x) - cameraPositionText.getLocalBounds().size.x -10.f,
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
            } else if (event->is<sf::Event::Resized>()) {
                const auto &resized = event->getIf<sf::Event::Resized>();
                window.setView(sf::View(sf::FloatRect(
                    {0.f, 0.f},
                    {static_cast<float>(resized->size.x), static_cast<float>(resized->size.y)}
                )));
                refreshDisplay();
            }
        }

    }
}


sf::Vector2f Projector::pointToVector2f(const Point3D &p) const {
    Point2D point2D = utils::project3DTo2D(camera.getCameraPosition(), p);
    //Zeby zgadzala sie skala po powiekszeniu okna
    const double scaleX = static_cast<double>(window.getSize().x) / screenWidth;
    const double scaleY = static_cast<double>(window.getSize().y) / screenHeight;
    const double scale = std::min(scaleX, scaleY);

    const double x2d = point2D.x * camera.getFocal() * scale
                + static_cast<double>(window.getSize().x) / 2;

    const double y2d = -point2D.y * camera.getFocal() * scale
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
        default:
            break;
    }
}


void Projector::onMouseWheelScrolled(const sf::Event::MouseWheelScrolled &event) {
    camera.zoom(event.delta*5);
}
