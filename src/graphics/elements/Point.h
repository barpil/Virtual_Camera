//
// Created by Bcom_ on 09.04.2026.
//

#ifndef WIRTUALNA_KAMERA_POINT_H
#define WIRTUALNA_KAMERA_POINT_H

#endif //WIRTUALNA_KAMERA_POINT_H


struct Point3D {
    double x;
    double y;
    double z;
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