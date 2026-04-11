//
// Created by Bcom_ on 09.04.2026.
//

#include <cmath>

#include "Camera.h"

#include <array>

#include "../../utils/3DSpaceUtils.h"

namespace {
    struct VectorTrio;

    LocalAxes recreateLocalAxesFromVectorEndPointsRUF(const Point3D &right, const Point3D &up, const Point3D &forward);

    LocalAxes recreateLocalAxesFromVectorEndPointsFRU(const Point3D &forward, const Point3D &right, const Point3D &up);

    VectorTrio recreateLocalAxesFromVectorEndPoints(Point3D v1, Point3D v2, Point3D v3);
}

void Camera::move(const double xChange, const double yChange, const double zChange) {
    this->cameraPosition.x += utils::applyPrecision(
        localOrientation.right.x * xChange + localOrientation.up.x * yChange + localOrientation.forward.x * zChange,
        movePrecision);
    this->cameraPosition.y += utils::applyPrecision(
        localOrientation.right.y * xChange + localOrientation.up.y * yChange + localOrientation.forward.y * zChange,
        movePrecision);
    this->cameraPosition.z += utils::applyPrecision(
        localOrientation.right.z * xChange + localOrientation.up.z * yChange + localOrientation.forward.z * zChange,
        movePrecision);
}

void Camera::rotate(const double xRotationDeg, const double yRotationDeg, const double zRotationDeg) {
    const double xRad = utils::degreesToRadians(utils::applyPrecision(xRotationDeg, rotationPrecision));
    const double yRad = utils::degreesToRadians(utils::applyPrecision(yRotationDeg, rotationPrecision));
    const double zRad = utils::degreesToRadians(utils::applyPrecision(zRotationDeg, rotationPrecision));

    //lewo prawo
    if (std::abs(yRad) > 1e-9) {
        localOrientation.forward = utils::rotateByAxis(localOrientation.forward, localOrientation.up, yRad);
        localOrientation.right = utils::rotateByAxis(localOrientation.right, localOrientation.up, yRad);
    }
    //gora dol
    if (std::abs(xRad) > 1e-9) {
        localOrientation.forward = utils::rotateByAxis(localOrientation.forward, localOrientation.right, xRad);
        localOrientation.up = utils::rotateByAxis(localOrientation.up, localOrientation.right, xRad);
    }

    //obrot
    if (std::abs(zRad) > 1e-9) {
        localOrientation.right = utils::rotateByAxis(localOrientation.right, localOrientation.forward, zRad);
        localOrientation.up = utils::rotateByAxis(localOrientation.up, localOrientation.forward, zRad);
    }

    //bo trzeba je naprawic po tych wszystkich operacjach (bo moga nie byc juz do siebie prostopadle np.)
    localOrientation = recreateLocalAxesFromVectorEndPointsFRU(localOrientation.forward, localOrientation.right,
                                                               localOrientation.up);
}

void Camera::zoom(double focalChange) {
    this->focal = std::max(5.0, std::min(5000.0, this->focal+focalChange));
}

bool Camera::levelLocalHorizon() {
    //sprawdzamy, czy ktoras ze wspolrzednych wektora osi up jest blisko 1 lub -1, bo
    //wtedy oznacza to, ze juz lezy na ktorej z osi swiata, czyli naszym lokalnym horyzoncie
    double absX = std::abs(localOrientation.up.x);
    double absY = std::abs(localOrientation.up.y);
    double absZ = std::abs(localOrientation.up.z);
    double maxComponent = absX;
    if (absY > maxComponent) maxComponent = absY;
    if (absZ > maxComponent) maxComponent = absZ;
    if (maxComponent > 0.999999) {
        return false;
    }


    //probujemy znalezc ktora z osi swiata jest najblizsza do naszej aktualnej osi up
    const std::array<Point3D, 6> worldAxes = {
        {
            {1, 0, 0}, {-1, 0, 0},
            {0, 1, 0}, {0, -1, 0},
            {0, 0, 1}, {0, 0, -1}
        }
    };
    Point3D nearestWorldUp = {0, 1, 0};
    double maxDot = -1.0;
    for (const auto &axis: worldAxes) {
        //iloczyn skalarny okresli jak bardzo nasza os pionowa jest rownolegla do danej osi swiata
        double dot = localOrientation.up.x * axis.x +
                     localOrientation.up.y * axis.y +
                     localOrientation.up.z * axis.z;
        if (dot > maxDot) {
            maxDot = dot;
            nearestWorldUp = axis;
        }
    }

    //obliczamy idealna os right poprzez iloczyn wektorowy naszej osi forward i wyznaczonej najblizszej osi up swiata
    Point3D idealRight = {
        nearestWorldUp.y * localOrientation.forward.z - nearestWorldUp.z * localOrientation.forward.y,
        nearestWorldUp.z * localOrientation.forward.x - nearestWorldUp.x * localOrientation.forward.z,
        nearestWorldUp.x * localOrientation.forward.y - nearestWorldUp.y * localOrientation.forward.x
    };

    double idealRightLen = std::sqrt(
        idealRight.x * idealRight.x + idealRight.y * idealRight.y + idealRight.z * idealRight.z);

    //jezeli przez jakis blad w odswiezeniu pozycji wektorow okazaloby sie ze os forward i znaleziona nearestWorldUp
    // sie pokrywaja to wtedy iloczyn wektorowy moze wyjsc zero i wtedy bylby blad dzielenia przez zero
    //(ale ta sytuacja jest raczej malo prawdopodobna)
    if (idealRightLen < 0.001) {
        //nie ma sensu poziomowac bo i tak osie sie nie zgadzaja (bo w poprawnym przypadku sytuacja
        //ze najblizsza os swiata do naszej osi up, jest os forward jest niemozliwa (bo nasze up i forward powinny byc prostopadle)
        return true;
    }

    //normalizujemy dlugosc wyznaczonego wektora right do 1
    idealRight.x /= idealRightLen;
    idealRight.y /= idealRightLen;
    idealRight.z /= idealRightLen;


    //upodabniamy nasz wektor right do wyznaczonego idealnego wektora right
    //stosujemy ulamkowy wspolczynnik zeby powoli przechodzilo do tego wyznaczonego right (im dalej od niego jestesmy tym szybciej i potem zwalniamy)
    double correctionSpeedFactor = 0.1;
    localOrientation.right.x += (idealRight.x - localOrientation.right.x) * correctionSpeedFactor;
    localOrientation.right.y += (idealRight.y - localOrientation.right.y) * correctionSpeedFactor;
    localOrientation.right.z += (idealRight.z - localOrientation.right.z) * correctionSpeedFactor;

    //bo trzeba je naprawic po tych wszystkich operacjach (bo moga nie byc juz do siebie prostopadle i nieznormalizowane np.)
    localOrientation = recreateLocalAxesFromVectorEndPointsRUF(localOrientation.right, localOrientation.up,
                                                               localOrientation.forward);
    return true;
}

namespace {
    struct VectorTrio {
        Point3D v1;
        Point3D v2;
        Point3D v3;
    };

    //odtworzenie right->up->front
    LocalAxes recreateLocalAxesFromVectorEndPointsRUF(const Point3D &right, const Point3D &up, const Point3D &forward) {
        auto [v1, v2, v3] = recreateLocalAxesFromVectorEndPoints(right, up, forward);
        return {v3, v2, v1};
    }

    //odtworzenie front->up->right
    LocalAxes recreateLocalAxesFromVectorEndPointsFRU(const Point3D &forward, const Point3D &right, const Point3D &up) {
        auto [v1, v2, v3] = recreateLocalAxesFromVectorEndPoints(forward, right, up);
        return {v1, v3, v2};
    }


    //odtworzenie ukladu wspolrzednych na podstawie punktow koncowych wektorow (bo poczatki sa zawsze w (0, 0 ,0)
    //kolejnosc odtwarzania wektorow ma znaczenie, poniewaz pozniejsze sa naprawiane na podstawie wczesniejszych
    //odtworzenie v1->v2->v3
    VectorTrio recreateLocalAxesFromVectorEndPoints(Point3D v1, Point3D v2, Point3D v3) {
        //normalizacja dlugosci wektorow osi do dlugosci 1
        auto norm = [](Point3D &v) {
            double l = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
            if (l > 1e-9) {
                v.x /= l;
                v.y /= l;
                v.z /= l;
            }
        };

        norm(v1);

        //otrzymanie poprawnego v2 (prostopadle do v1) z cross productu  v1 i v3
        v2 = {
            v3.y * v1.z - v3.z * v1.y,
            v3.z * v1.x - v3.x * v1.z,
            v3.x * v1.y - v3.y * v1.x
        };
        norm(v2);

        //otrzymanie poprawnego v3 (prostopadle zarowno do v1 i v2) z cross productu  v1 i v2
        v3 = {
            v1.y * v2.z - v1.z * v2.y,
            v1.z * v2.x - v1.x * v2.z,
            v1.x * v2.y - v1.y * v2.x
        };
        norm(v3);
        return {v1, v2, v3};
    }
}
