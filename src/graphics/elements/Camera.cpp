//
// Created by Bcom_ on 09.04.2026.
//

#include <cmath>

#include "Camera.h"

#include "../../utils/3DSpaceUtils.h"

struct Pair {
    double v1, v2;
};

void Camera::move(const double xChange, const double yChange, const double zChange) {
    const Point3D localMove = {xChange, yChange, zChange};

    auto [x, y, z] = utils::rotate3D(localMove,
                                     cameraRotation.xRotation,
                                     cameraRotation.yRotation,
                                     cameraRotation.zRotation);

    this->cameraPosition.x += x;
    this->cameraPosition.y += y;
    this->cameraPosition.z += z;
}


//Do przerobienia tak zeby uzywaly macierzy Rotacji, a nie 3 koordynatow, bo przez to ze nie ma macierzy rotacji
//nie da się zrobić obrotu wzgledem kamery. Aktualnie dziala obrot wzgledem srodka ukladu wspolrzednych
//Zakres obrotu to od <0; 360)
void Camera::rotate(const double xRotation, const double yRotation, const double zRotation) {
    cameraRotation.xRotation = std::fmod(cameraRotation.xRotation + xRotation, 360.0);
    cameraRotation.yRotation = std::fmod(cameraRotation.yRotation + yRotation, 360.0);
    cameraRotation.zRotation = std::fmod(cameraRotation.zRotation + zRotation, 360.0);
}

void Camera::zoom(double focalChange) {
    this->focal += focalChange;
}
