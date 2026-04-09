//
// Created by Bcom_ on 09.04.2026.
//

#ifndef WIRTUALNA_KAMERA_CAMERA_H
#define WIRTUALNA_KAMERA_CAMERA_H
#include "Figure.h"

enum class Sign: int {
    POSITIVE = 1,
    NEGATIVE = -1
};
//Operatory żeby podstawowe * działało dla double*enum
inline double operator*(const double lhs, const Sign rhs) {
    return lhs * static_cast<double>(rhs);
}
inline double operator*(const Sign lhs, const double rhs) {
    return rhs * lhs;
}


struct Direction {
    Sign xDirection;
    Sign yDirection;
    Sign zDirection;
};

class Camera {
    Point3D cameraPosition;
    Direction direction;
    double focal;
public:
    Camera(const Point3D &initialPosition, const Direction initialDirection, const double initialFocal) :
        cameraPosition(initialPosition),direction(initialDirection), focal(initialFocal) {

    }

    [[nodiscard]] Point3D getCameraPosition() const {
        return cameraPosition;
    }

    [[nodiscard]] Direction getDirection() const {
        return direction;
    }

    [[nodiscard]] double getFocal() const {
        return focal;
    }

    void move(double xChange, double yChange, double zChange);
    void rotate(double xRotation, double yRotation, double zRotation);
    void zoom(double focalChange);

};



#endif //WIRTUALNA_KAMERA_CAMERA_H
