//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_3DSPACEUTILS_H
#define WIRTUALNA_KAMERA_3DSPACEUTILS_H

#include "../graphics/elements/Figure.h"

namespace utils {
    double distance3D(const Point3D& p1, const Point3D& p2);
    Point2D project3DTo2D(const Point3D &cameraPosition, const Point3D &point);
}


#endif //WIRTUALNA_KAMERA_3DSPACEUTILS_H
