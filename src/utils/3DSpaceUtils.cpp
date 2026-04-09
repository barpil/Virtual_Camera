//
// Created by Bcom_ on 04.04.2026.
//
#include "3DSpaceUtils.h"

#include <cmath>

#include "../graphics/elements/Figure.h"

//Przeczytałem coś, że takie liczenie odleglosci w przestrzeni euklidesowej moze byc niepoprawne, bo powoduje
//efekt rybiego oka. Ale jeszcze tego nie sprawdzałem

namespace utils{
    double distance3D(const Point3D &p1, const Point3D &p2) {
        const double dx = p2.x-p1.x;
        const double dy = p2.y-p1.y;
        const double dz = p2.z-p1.z;

        return sqrt((dx*dx)+(dy*dy)+(dz*dz));
    }


    Point2D project3DTo2D(const Point3D &cameraPosition, const Point3D &point) {
        const double d = distance3D(point, cameraPosition);
        //point position relative to camera
        const double rx = point.x - cameraPosition.x;
        const double ry = point.y - cameraPosition.y;
        const double rz = point.z - cameraPosition.z;

        Point2D resultPoint{};
        resultPoint.x = (rx*d)/(rz+d);
        resultPoint.y = (ry*d)/(rz+d);
        return resultPoint;
    }
}


