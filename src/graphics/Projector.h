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
    std::vector<sf::VertexArray> vertexesVector;
    std::vector<std::vector<Point3DWithColor>> projectedFigures;
    sf::Text cameraPositionText;
    sf::Font font;
    Camera camera;
    std::vector<Figure> figures;

public:
    Projector(int screenWidth, int screenHeight, const Camera &camera);


    void render(const std::vector<Figure> &figureList);
    void refreshDisplay();
    void clearLineBuffer();

    [[nodiscard]] Camera getCamera() const {
        return camera;
    }

private:
    void preparePolygonsInGeneralSpace(const std::vector<Figure> &figureList);
    std::optional<Line2D> projectLine(const Point3D &point1, const Point3D &point2) const;
    std::optional<std::vector<Point3D>> projectWall(const std::vector<Point3D> &points) const;

    void drawElement(const std::vector<Point3DWithColor> &elementPointsVector);
    void drawLine(const std::vector<Point3DWithColor> &elementPointsVector);
    void drawPolygon(const std::vector<Point3DWithColor> &elementPointsVector);



    void redrawFigures();
    void drawFigureNet(const Figure &figure);
    void drawFigureWalls(const Figure &figure);
    void refreshOnScreenText();

    bool handleKeyboardInput(float dt);
    void onMouseWheelScrolled(const sf::Event::MouseWheelScrolled &event);
    void onDrag(Point2D dragStart, Point2D dragEnd);
    void takePhoto();
};



#endif //WIRTUALNA_KAMERA_PROJECTOR_H
