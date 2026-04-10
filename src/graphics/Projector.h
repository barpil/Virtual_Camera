//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_PROJECTOR_H
#define WIRTUALNA_KAMERA_PROJECTOR_H
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>

#include "elements/Camera.h"
#include "elements/Figure.h"




class Projector {
    int screenWidth;
    int screenHeight;
    sf::RenderWindow window;
    sf::VertexArray lines;
    sf::Text cameraPositionText;
    sf::Font font;
    Camera camera;
    std::vector<Figure> figures;

public:
    Projector(int screenWidth, int screenHeight, const Camera &camera);

    void drawFigure(const Figure &figure);
    void render(const std::vector<Figure> &figureList);
    void refreshDisplay();
    void clearLineBuffer();

    [[nodiscard]] Camera getCamera() const {
        return camera;
    }

private:
    sf::Vector2f projectPoint(const Point3D &p) const;
    void redrawFigures();

    void refreshOnScreenText();

    void onKeyPressed(sf::Keyboard::Key key);
    void onMouseWheelScrolled(const sf::Event::MouseWheelScrolled &event);
    void onDrag(Point2D dragStart, Point2D dragEnd);
};



#endif //WIRTUALNA_KAMERA_PROJECTOR_H
