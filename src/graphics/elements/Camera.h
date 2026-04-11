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
    double zLimit;
    double moveSpeed;
    double rotationSpeed;

public:
    Camera(const Point3D &initialPosition, const double initialFocal, const double moveSpeed, const double rotationSpeed, const double rotationPrecision, const double movePrecision, const double zLimit) :
        cameraPosition(initialPosition), focal(initialFocal), moveSpeed(moveSpeed), rotationSpeed(rotationSpeed), rotationPrecision(rotationPrecision), movePrecision(movePrecision), zLimit(zLimit) {
    }

    [[nodiscard]] Point3D getCameraPosition() const {
        return cameraPosition;
    }

    [[nodiscard]] LocalAxes getLocalOrientation() const {
        return localOrientation;
    }

    [[nodiscard]] double getMoveSpeed() const {
        return moveSpeed;
    }

    [[nodiscard]] double getRotationSpeed() const {
        return rotationSpeed;
    }

    [[nodiscard]] double getFocal() const {
        return focal;
    }

    [[nodiscard]] double getZClippingValue() const {
        return zLimit*focal;
    }

    void move(double xChange, double yChange, double zChange);
    void rotate(double xRotation, double yRotation, double zRotation);
    void zoom(double focalChange);
    bool levelLocalHorizon();

};



#endif //WIRTUALNA_KAMERA_CAMERA_H
