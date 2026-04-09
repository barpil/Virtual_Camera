//
// Created by Bcom_ on 09.04.2026.
//

#include "Camera.h"



void Camera::move(const double xChange, const double yChange, const double zChange) {
    Point3D newPosition{};
    newPosition.x = this->cameraPosition.x + xChange*this->direction.xDirection;
    newPosition.y = this->cameraPosition.y + yChange*this->direction.yDirection;
    newPosition.z = this->cameraPosition.z + zChange*this->direction.zDirection;
    this->cameraPosition = newPosition;
}

void Camera::rotate(double xRotation, double yRotation, double zRotation) {
    //Do zaimplementowania
}

void Camera::zoom(double focalChange) {
    this->focal += focalChange;
}
