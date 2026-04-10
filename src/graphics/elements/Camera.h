//
// Created by Bcom_ on 09.04.2026.
//

#ifndef WIRTUALNA_KAMERA_CAMERA_H
#define WIRTUALNA_KAMERA_CAMERA_H
#include "Figure.h"


struct LocalAxes {
    Point3D forward;
    Point3D up;
    Point3D right;
};

class Camera {
    Point3D cameraPosition;
    LocalAxes localOrientation = {{0, 0, 1}, {0, 1, 0}, {1, 0 ,0}};
    double focal;
    double rotationPrecision;
    double movePrecision;

public:
    Camera(const Point3D &initialPosition, const double initialFocal, const double rotationPrecision, const double movePrecision) :
        cameraPosition(initialPosition), focal(initialFocal), rotationPrecision(rotationPrecision), movePrecision(movePrecision) {
    }

    [[nodiscard]] Point3D getCameraPosition() const {
        return cameraPosition;
    }

    [[nodiscard]] LocalAxes getLocalOrientation() const {
        return localOrientation;
    }

    [[nodiscard]] double getFocal() const {
        return focal;
    }

    void move(double xChange, double yChange, double zChange);
    void rotate(double xRotation, double yRotation, double zRotation);
    void zoom(double focalChange);
    bool levelLocalHorizon();

};



#endif //WIRTUALNA_KAMERA_CAMERA_H
