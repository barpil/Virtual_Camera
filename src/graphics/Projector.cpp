//
// Created by Bcom_ on 04.04.2026.
//

#include <SFML/System/Vector2.hpp>

#include "Projector.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "../utils/3DSpaceUtils.h"


Projector::Projector(const int screenWidth, const int screenHeight, const double fieldOfView, const Point &initialCameraPosition)
: screenWidth(screenWidth), screenHeight(screenHeight), fieldOfView(fieldOfView), cameraPosition(initialCameraPosition) {

    window.create(sf::VideoMode({static_cast<unsigned>(screenWidth), static_cast<unsigned>(screenHeight)}), "Virutal Camera");
    window.setFramerateLimit(60);

    lines.setPrimitiveType(sf::PrimitiveType::Lines);

}



void Projector::drawFigure(const Figure &figure) {
    for (const Edge& edge : figure.edges) {
        const sf::Vector2f p1 = pointToVector2f(figure.points[edge.start], fieldOfView, cameraPosition);
        const sf::Vector2f p2 = pointToVector2f(figure.points[edge.end], fieldOfView, cameraPosition);

        lines.append(sf::Vertex{p1, sf::Color::Green});
        lines.append(sf::Vertex{p2, sf::Color::Green});
    }
}

void Projector::refreshDisplay() {
    window.clear();
    window.draw(lines);
    window.display();
}

void Projector::clearLineBuffer() {
    lines.clear();
}

void Projector::render() {
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
        }

        refreshDisplay();
    }
}


//Wzor na rzutowanie 3D na 2D wziety z https://4programmers.net/Forum/Algorytmy/309761-rzutowanie_perspektywiczne_przeksztalcenie_wierzcholka_3d_w_punkt_2d

sf::Vector2f Projector::pointToVector2f(const Point &p, const double fieldOfView, const Point &cameraPosition) const {
    const double distanceFromCenterOfCS = utils::distance3D(p, cameraPosition);
    const double factor = fieldOfView / (p.z + distanceFromCenterOfCS);

    const double x2d = p.x * factor + static_cast<double>(screenWidth) / 2;
    const double y2d = -p.y * factor + static_cast<double>(screenHeight) / 2; //z minusem bo SFML y rośnie od góry do dołu
    return {static_cast<float>(x2d), static_cast<float>(y2d)};
}
