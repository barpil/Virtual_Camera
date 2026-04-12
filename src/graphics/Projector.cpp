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
#include "../utils/MySFMLUtils.h"
#include "binaries/RobotoMonoFont.h"
#include "elements/Camera.h"

const double ARCBALL_SENSITIVITY = 1;

Projector::Projector(const int screenWidth, const int screenHeight, const Camera &camera)
    : screenWidth(screenWidth), screenHeight(screenHeight), cameraPositionText(font), camera(camera) {
    window.create(sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}),
                  "Virutal Camera");
    window.setFramerateLimit(60);

    if (!font.openFromMemory(RobotoMonoFont_ttf, RobotoMonoFont_ttf_len)) {
        std::cerr << "Failed to load font\n";
    }
    cameraPositionText.setCharacterSize(16);
}

//minZ to kiedy osiagamy pelna jasnosc, a maxZ to kiedy pelna ciemnosc
sf::Color calculateDepthColor(const sf::Color baseColor, const double z, double minZ, double maxZ) {
    //mapujemy polozenie z na wspolczynnik glebokosci
    double factor = utils::calculateDepthFactor(z, minZ, maxZ, 0.3, 1, 0.7);

    return {
        static_cast<uint8_t>(baseColor.r * factor),
        static_cast<uint8_t>(baseColor.g * factor),
        static_cast<uint8_t>(baseColor.b * factor)
    };
}

void Projector::drawFigure(const Figure &figure) {
    for (const auto &[start, end]: figure.edges) {
        sf::VertexArray vertexes;
        vertexes.setPrimitiveType(sf::PrimitiveType::TriangleFan);
        auto projectedLine = projectLine(figure.points[start], figure.points[end]);
        if (!projectedLine) continue;

        const Line2D line = projectedLine.value();
        const Line2DWithThickness lineWithThickness = utils::createLine2DWithThicknessFromLine(line, 1, 15, camera.getFocal());

        const sf::Vector2f v1 = {static_cast<float>(lineWithThickness.startL.x), static_cast<float>(lineWithThickness.startL.y)};
        const sf::Vector2f v2 = {static_cast<float>(lineWithThickness.startR.x), static_cast<float>(lineWithThickness.startR.y)};
        const sf::Vector2f v3 = {static_cast<float>(lineWithThickness.endL.x), static_cast<float>(lineWithThickness.endL.y)};
        const sf::Vector2f v4 = {static_cast<float>(lineWithThickness.endR.x), static_cast<float>(lineWithThickness.endR.y)};

        //uzaleznienie koloru od oddalenia punktu
        const sf::Color color1 = calculateDepthColor(figure.color, line.startZDepth, 0.3, 2);
        const sf::Color color2 = calculateDepthColor(figure.color, line.endZDepth, 0.3, 2);

        vertexes.append(sf::Vertex{v1, color1});
        vertexes.append(sf::Vertex{v2, color1});
        vertexes.append(sf::Vertex{v3, color2});
        vertexes.append(sf::Vertex{v4, color2});

        vertexesVector.push_back(vertexes);
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
        "Camera:\nPosition: (x:{:.2f}; y:{:.2f}; z:{:.2f})\nRotation: (ax:{:.2f}; ay:{:.2f}; az:{:.2f})\nFocal   : {}",
        camera.getCameraPosition().x,
        camera.getCameraPosition().y, camera.getCameraPosition().z,
        //Przeliczenie pozycji osi kamery na rotacje w stopniach
        std::asin(-camera.getLocalOrientation().forward.y) * 180.0 / M_PI,
        std::atan2(camera.getLocalOrientation().forward.x, camera.getLocalOrientation().forward.z) * 180.0 / M_PI,
        std::atan2(camera.getLocalOrientation().right.y, camera.getLocalOrientation().up.y) * 180.0 / M_PI,
        camera.getFocal())
        );
    cameraPositionText.setPosition({
        static_cast<float>(window.getSize().x) - cameraPositionText.getLocalBounds().size.x - 10.f,
        10.f
    });
    window.draw(cameraPositionText);
}

void Projector::clearLineBuffer() {
    vertexesVector.clear();
}

void Projector::redrawFigures() {
    clearLineBuffer();
    for (Figure const &figure: figures) {
        drawFigure(figure);
    }
    for (sf::VertexArray va : vertexesVector) {
        window.draw(va);
    }
}

void Projector::render(const std::vector<Figure> &figureList) {
    this->figures = figureList;
    refreshDisplay();
    std::optional<sf::Vector2i> dragStart;
    bool isDragging = false;
    int keysPressedCount = 0;
    bool canTakePhoto = true;
    sf::Clock clock;
    bool refreshNeeded = false;
    while (window.isOpen()) {
        sf::Time elapsed = clock.restart();
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            else if (event->is<sf::Event::KeyPressed>()) {
                keysPressedCount++; //ale to sprawdzanie czy jest wcisniety trzeba na koniec dac
                if (canTakePhoto && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) {
                    takePhoto();
                    canTakePhoto = false;
                }
            } else if (event->is<sf::Event::KeyReleased>()) {
                keysPressedCount--;
                if (!canTakePhoto && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::P)) canTakePhoto = true;
            }else if (event->is<sf::Event::MouseWheelScrolled>()) {
                onMouseWheelScrolled(*event->getIf<sf::Event::MouseWheelScrolled>());
                refreshNeeded = true;
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
                refreshNeeded = true;
            }
        }

        if (isDragging) {
            sf::Vector2i dragEnd = sf::Mouse::getPosition(window);
            if (dragStart) {
                onDrag({static_cast<double>(dragStart->x), static_cast<double>(dragStart->y)}, {
                           static_cast<double>(dragEnd.x), static_cast<double>(dragEnd.y)
                       });
                refreshNeeded = true;
                dragStart = dragEnd;
            }
        }

        if (keysPressedCount > 0 && handleKeyboardInput(elapsed.asSeconds())) refreshNeeded = true;


        if (refreshNeeded) {
            refreshDisplay();
            refreshNeeded = false;
        }
    }
}


std::optional<Line2D> Projector::projectLine(const Point3D &point1, const Point3D &point2) const {

    Point3D p1 = point1;
    Point3D p2 = point2;

    utils::transformPointToPointInCameraLocalAxes(p1, camera.getCameraPosition(), camera.getLocalOrientation());
    utils::transformPointToPointInCameraLocalAxes(p2, camera.getCameraPosition(), camera.getLocalOrientation());
    if (!utils::performLineZClipping(p1, p2, camera.getZClippingValue())) return std::nullopt;

    Point2D point2D1 = utils::project3DTo2D(p1, camera.getFocal());
    Point2D point2D2 = utils::project3DTo2D(p2, camera.getFocal());


    //Zeby zgadzala sie skala po powiekszeniu okna
    const double scaleX = static_cast<double>(window.getSize().x) / screenWidth;
    const double scaleY = static_cast<double>(window.getSize().y) / screenHeight;
    const double scale = std::min(scaleX, scaleY);


    double windowHalfedX = static_cast<double>(window.getSize().x) / 2;
    double windowHalfedY = static_cast<double>(window.getSize().y) / 2;

    return {{
        {point2D1.x * scale + windowHalfedX, -point2D1.y * scale + windowHalfedY},
        {point2D2.x * scale + windowHalfedX, -point2D2.y * scale + windowHalfedY},
        p1.z, p2.z
    }};
}


void Projector::takePhoto() {
    auto now = std::chrono::system_clock::now();
    std::string outputFilePath = std::format("./photo-{:%Y%m%d_%H%M%S}.png", now);
    sf::Texture texture;
    if (!texture.resize({window.getSize().x, window.getSize().y})) {
        std::cerr << "An error occured while trying to take photo." << std::endl;
        return;
    }
    window.clear();
    redrawFigures();
    texture.update(window);
    if (const sf::Image screenshot = texture.copyToImage(); screenshot.saveToFile(outputFilePath)) {
        std::cout << std::format("Photo saved at: {}", outputFilePath) << std::endl;
    } else {
        std::cerr << "An error occured while trying to save taken photo." << std::endl;
    }
}


bool Projector::handleKeyboardInput(const float dt) {
    const double moveSpeed = camera.getMoveSpeed() * dt;
    const double rotateSpeed = camera.getRotationSpeed() * dt;


    bool refreshNeeded = false;
    //ruch
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
        camera.move(0, 0, moveSpeed);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
        camera.move(0, 0, -moveSpeed);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        camera.move(-moveSpeed, 0, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        camera.move(moveSpeed, 0, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        camera.move(0, moveSpeed, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        camera.move(0, -moveSpeed, 0);
        refreshNeeded = true;
    }

    //wyrownanie horyzontu
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
        camera.levelLocalHorizon();
        refreshNeeded = true;
    }

    //arcball z klawiatury
    const Point2D screenCenter = {
        static_cast<double>(window.getSize().x) / 2, 
        static_cast<double>(window.getSize().y) / 2
    };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
        auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
            screenCenter, {screenCenter.x, screenCenter.y + 10}, 
            window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
        camera.rotate(xRotation, yRotation, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) {
        auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
            screenCenter, {screenCenter.x, screenCenter.y - 10}, 
            window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
        camera.rotate(xRotation, yRotation, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)) {
        auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
            screenCenter, {screenCenter.x - 10, screenCenter.y}, 
            window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
        camera.rotate(xRotation, yRotation, 0);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)) {
        auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
            screenCenter, {screenCenter.x + 10, screenCenter.y}, 
            window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
        camera.rotate(xRotation, yRotation, 0);
        refreshNeeded = true;
    }

    //beczka
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
        camera.rotate(0, 0, rotateSpeed);
        refreshNeeded = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::E)) {
        camera.rotate(0, 0, -rotateSpeed);
        refreshNeeded = true;
    }



    if (refreshNeeded) {
        return true;
    }
    return false;
}


void Projector::onMouseWheelScrolled(const sf::Event::MouseWheelScrolled &event) {
    camera.zoom(event.delta * 5);
}

void Projector::onDrag(Point2D dragStart, Point2D dragEnd) {
    auto [xRotation, yRotation, zRotation] = utils::calculateRotationWithArcball(
        dragStart, dragEnd, window.getSize().x, window.getSize().y, ARCBALL_SENSITIVITY);
    camera.rotate(xRotation, yRotation, 0); //Rotacja Z ustawiana na 0, bo inaczej zbyt ciezko sterowac
}
