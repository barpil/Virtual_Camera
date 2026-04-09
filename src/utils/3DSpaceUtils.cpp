//
// Created by Bcom_ on 04.04.2026.
//
#include "3DSpaceUtils.h"

#include <algorithm>
#include <cmath>

#include "../graphics/elements/Camera.h"
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

    double degreesToRadians(const double degrees) {
        return degrees * M_PI / 180.0;
    }


    namespace {
        void rotate3DX(Point3D &p, const double degrees) {
            const double rad = degreesToRadians(degrees);
            const double y = p.y;
            const double z = p.z;
            p.y = y*std::cos(rad) - z*std::sin(rad);
            p.z = y*std::sin(rad) + z*std::cos(rad);
        }

        void rotate3DY(Point3D &p, const double degrees) {
            const double rad = degreesToRadians(degrees);
            const double x = p.x;
            const double z = p.z;
            p.x = x*std::cos(rad) + z*std::sin(rad);
            p.z = -x*std::sin(rad) + z*std::cos(rad);
        }

        void rotate3DZ(Point3D &p, const double degrees) {
            const double rad = degreesToRadians(degrees);
            const double x = p.x;
            const double y = p.y;
            p.x = x*std::cos(rad) - y*std::sin(rad);
            p.y = x*std::sin(rad) + y*std::cos(rad);
        }
    }


    Point3D rotate3D(const Point3D &p, const double xDegrees, const double yDegrees, const double zDegrees) {
        Point3D resultPoint = p;
        rotate3DX(resultPoint, xDegrees);
        rotate3DY(resultPoint, yDegrees);
        rotate3DZ(resultPoint, zDegrees);
        return resultPoint;
    }

    Rotation calculateRotationWithArcball(const Point2D &startPosition, const Point2D &endPosition, const int windowWidth, const int windowHeight) {
        double centerX = windowWidth / 2.0;
        double centerY = windowHeight / 2.0;

        double radius = (std::min(windowWidth, windowHeight) / 2.0);

        auto mapToSphere = [&](const Point2D& pos) -> Point3D {
            double x = (pos.x - centerX) / radius;
            double y = (centerY - pos.y) / radius;

            double mag2 = x*x + y*y;
            if (mag2 <= 1.0) {
                return {x, y, std::sqrt(1.0 - mag2)};
            }
            double mag = std::sqrt(mag2);
            return {x / mag, y / mag, 0.0};
        };

        Point3D vStart = mapToSphere(startPosition);
        Point3D vEnd = mapToSphere(endPosition);

        double dot = vStart.x * vEnd.x + vStart.y * vEnd.y + vStart.z * vEnd.z;
        dot = std::clamp(dot, -1.0, 1.0);
        double angleRad = std::acos(dot);

        if (angleRad < 1e-6) return {0.0, 0.0, 0.0};

        Point3D axis = {
            vStart.y * vEnd.z - vStart.z * vEnd.y,
            vStart.z * vEnd.x - vStart.x * vEnd.z,
            vStart.x * vEnd.y - vStart.y * vEnd.x
        };

        double len = std::sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
        if (len > 0) { axis.x /= len; axis.y /= len; axis.z /= len; }

        const double radToDeg = 180.0 / M_PI;
        return {
            axis.x * angleRad * radToDeg,
            axis.y * angleRad * radToDeg,
            axis.z * angleRad * radToDeg
        };
    }


    Point2D project3DTo2D(const Point3D &cameraPosition, const Rotation &cameraRotation, const double focal, const Point3D &point) {
        Point3D relativePoint = {
            point.x - cameraPosition.x,
            point.y - cameraPosition.y,
            point.z - cameraPosition.z
        };

        Point3D p = relativePoint;

        p = rotate3D(p, -cameraRotation.xRotation, -cameraRotation.yRotation, -cameraRotation.xRotation);


        // Zabezpieczenie przed podzieleniem przez p.z bliskiemu 0
        if (p.z <= 0.1) return {0, 0};

        Point2D resultPoint{};
        resultPoint.x = (p.x * focal) / p.z;
        resultPoint.y = (p.y * focal) / p.z;

        return resultPoint;
    }




}




