//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_PROJECTOR_H
#define WIRTUALNA_KAMERA_PROJECTOR_H
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include "Figure.h"


class Projector {
    int screenWidth;
    int screenHeight;
    sf::RenderWindow window;
    sf::VertexArray lines;
    double fieldOfView;
    Point cameraPosition;

public:
    Projector(int screenWidth, int screenHeight, double fieldOfView, const Point &cameraPosition);

    void drawFigure(const Figure &figure);
    void render();
    void refreshDisplay();
    void clearLineBuffer();
private:
    sf::Vector2f pointToVector2f(const Point &p, double fieldOfView, const Point &cameraPosition) const;
};



#endif //WIRTUALNA_KAMERA_PROJECTOR_H
