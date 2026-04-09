//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_3DSPACEUTILS_H
#define WIRTUALNA_KAMERA_3DSPACEUTILS_H

#include "../graphics/elements/Figure.h"

struct Rotation;

namespace utils {
    double distance3D(const Point3D& p1, const Point3D& p2);
    Point2D project3DTo2D(const Point3D &cameraPosition, const Rotation &cameraRotation, const Point3D &point);
    double degreesToRadians(double degrees);
    Rotation calculateRotationWithArcball(const Point2D &startPosition, const Point2D &endPosition, const int windowWidth, const int windowHeight);
}


#endif //WIRTUALNA_KAMERA_3DSPACEUTILS_H
