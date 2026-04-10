//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_3DSPACEUTILS_H
#define WIRTUALNA_KAMERA_3DSPACEUTILS_H

#include "../graphics/elements/Figure.h"
#include "../graphics/elements/Camera.h"

struct Rotation {
    double xRotation;
    double yRotation;
    double zRotation;
};

namespace utils {
    double distance3D(const Point3D& p1, const Point3D& p2);
    Point2D project3DTo2D(const Point3D &cameraPosition, const LocalAxes &cameraOrientation, const double focal, const Point3D &point);
    double degreesToRadians(double degrees);
    Point3D rotate3D(const Point3D &p, const double xDegrees, const double yDegrees, const double zDegrees);
    Rotation calculateRotationWithArcball(const Point2D &startPosition, const Point2D &endPosition, const int windowWidth, const int windowHeight, const double sensitivity);
    double applyPrecision(const double value, const double precision);
    Point3D rotateByAxis(const Point3D &point, const Point3D &axis, const double angleRad);
    void normalizeVector(Point3D &vector);
}


#endif //WIRTUALNA_KAMERA_3DSPACEUTILS_H
