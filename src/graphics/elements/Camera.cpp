//
// Created by Bcom_ on 09.04.2026.
//

#include <cmath>

#include "Camera.h"



void Camera::move(const double xChange, const double yChange, const double zChange) {
    Point3D newPosition{};
    newPosition.x = this->cameraPosition.x + xChange*this->direction.xDirection;
    newPosition.y = this->cameraPosition.y + yChange*this->direction.yDirection;
    newPosition.z = this->cameraPosition.z + zChange*this->direction.zDirection;
    this->cameraPosition = newPosition;
}

//Zakres obrotu to od <0; 360)
void Camera::rotate(const double xRotation, const double yRotation, const double zRotation) {
    cameraRotation.xRotation = std::fmod(cameraRotation.xRotation+xRotation, 360.0);
    cameraRotation.yRotation = std::fmod(cameraRotation.yRotation+yRotation, 360.0);
    cameraRotation.zRotation = std::fmod(cameraRotation.zRotation+zRotation, 360.0);
}

void Camera::zoom(double focalChange) {
    this->focal += focalChange;
}
