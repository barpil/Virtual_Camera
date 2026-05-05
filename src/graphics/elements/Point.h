//
// Created by Bcom_ on 09.04.2026.
//

#ifndef WIRTUALNA_KAMERA_POINT_H
#define WIRTUALNA_KAMERA_POINT_H
#include <SFML/Graphics/Color.hpp>

struct Point3D {
    double x;
    double y;
    double z;
};

struct Point3DWithColor : Point3D  {
    sf::Color color;
};

struct Point2D {
    double x;
    double y;
};

struct Line2D {
    Point2D start;
    Point2D end;
    double startZDepth;
    double endZDepth;
};

struct Line2DWithThickness {
    Point2D startL;
    Point2D startR;
    Point2D endL;
    Point2D endR;
    double startZDepth;
    double endZDepth;
};

#endif //WIRTUALNA_KAMERA_POINT_H


