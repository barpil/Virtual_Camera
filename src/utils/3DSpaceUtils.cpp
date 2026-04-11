//
// Created by Bcom_ on 04.04.2026.
//
#include "3DSpaceUtils.h"

#include <algorithm>
#include <cmath>

#include "../graphics/elements/Camera.h"
#include "../graphics/elements/Figure.h"

namespace utils {
    double distance3D(const Point3D &p1, const Point3D &p2) {
        const double dx = p2.x - p1.x;
        const double dy = p2.y - p1.y;
        const double dz = p2.z - p1.z;

        return sqrt((dx * dx) + (dy * dy) + (dz * dz));
    }

    double degreesToRadians(const double degrees) {
        return degrees * M_PI / 180.0;
    }


    //Obliczenie rotacji wektora wedlug osi z formuly Rodriguesa
    // v_rot = v*cosA + (axis x v)*sinA + axis*(axis . v)*(1 - cosA)
    Point3D rotateByAxis(const Point3D &point, const Point3D &axis, const double angleRad) {
        // axis . v -> (iloczyn skalarny)
        double dot = axis.x * point.x + axis.y * point.y + axis.z * point.z;
        // axis x v -> (iloczyn wektorowy)
        Point3D cross = {
            axis.y * point.z - axis.z * point.y,
            axis.z * point.x - axis.x * point.z,
            axis.x * point.y - axis.y * point.x
        };

        double cosA = std::cos(angleRad);
        double sinA = std::sin(angleRad);
        return {
            point.x * cosA + cross.x * sinA + axis.x * dot * (1 - cosA),
            point.y * cosA + cross.y * sinA + axis.y * dot * (1 - cosA),
            point.z * cosA + cross.z * sinA + axis.z * dot * (1 - cosA)
        };
    }


    //Rotacja wzgledem osi swiata
    Point3D rotate3D(const Point3D &p, double xDeg, double yDeg, double zDeg) {
        Point3D result = p;

        //os x
        if (std::abs(xDeg) > 0.0001) {
            result = rotateByAxis(result, {1, 0, 0}, xDeg);
        }
        //os y
        if (std::abs(yDeg) > 0.0001) {
            result = rotateByAxis(result, {0, 1, 0}, yDeg);
        }
        //os z
        if (std::abs(zDeg) > 0.0001) {
            result = rotateByAxis(result, {0, 0, 1}, zDeg);
        }

        return result;
    }

    Rotation calculateRotationWithArcball(const Point2D &startPosition, const Point2D &endPosition,
                                          const int windowWidth, const int windowHeight, const double sensitivity) {
        //srodek kola z ktorego mapujemy
        const double centerX = windowWidth / 2.0;
        const double centerY = windowHeight / 2.0;
        //wyznaczamy promien kola z ktorego mapujemy (minimum bierzemy, zeby za okienko nie wychodzilo)
        const double radius = std::min(windowWidth, windowHeight) * 0.8;

        //mapujemy punkt na sfere wyznaczajac wspolrzedna z w oparciu o x i y
        auto mapToSphere = [&](const Point2D &pos) -> Point3D {
            //normalizacja wspolrzednych do 1 wzgl srodka sfery
            double x = (pos.x - centerX) / radius;
            double y = -(pos.y - centerY) / radius; //z minusem bo w SFML y rosnie w dol

            double mag2 = x * x + y * y;
            //odleglosc zmapowanego punktu od srodka w plaszczyznie XY (jezeli >1 to wtedy poza kolem kliknieto, a w takim przypadku nie da sie tego punktu zmapowac na sfere)
            if (mag2 <= 1.0) {
                double z = std::sqrt(1.0 - mag2);
                return {x, y, z};
            }
            //jak sie nie da zmapowac na sfere to brak obrotu
            return {0, 0, 0};
        };

        //mapujemy pozycję startową i końcową myszki na punkty na sferze
        Point3D vStart = mapToSphere(startPosition);
        Point3D vEnd = mapToSphere(endPosition);

        //obliczamy kat obrotu z iloczynu skalarnego i przycinamy wartosc do [-1, 1]
        const double dot = std::clamp(vStart.x * vEnd.x + vStart.y * vEnd.y + vStart.z * vEnd.z, -1.0, 1.0);
        const double angleRad = std::acos(dot);
        //dla zbyt malych obrotow pomijamy
        if (angleRad < 1e-4) return {0.0, 0.0, 0.0};

        //wyznaczamy os obrotu z iloczynu wektorowego punktow na sferze
        Point3D axis = {
            vStart.y * vEnd.z - vStart.z * vEnd.y,
            vStart.z * vEnd.x - vStart.x * vEnd.z,
            vStart.x * vEnd.y - vStart.y * vEnd.x
        };
        //normalizujemy wyznaczona os
        normalizeVector(axis);

        //przeksztalcenie z radianow na katy obrotu w osi
        const double finalAngleDeg = (angleRad * 180.0 / M_PI) * sensitivity;
        return {
            axis.x * finalAngleDeg,
            axis.y * finalAngleDeg,
            axis.z * finalAngleDeg
        };
    }

    //wystarczy clipping Z, bo SFML dokonuje automatycznie clippingu elementow, które wychodzą za view port
    //zwraca bool, czy warto rysowac te linie, czy jednak nie ma sensu
    bool performLineZClipping(Point3D &p1, Point3D &p2, const double zLimit) {
        //jezeli oba punkty sa za kamera to ich nie rysujemy
        if (p1.z<zLimit && p2.z<zLimit) return false;
        //jezeli jeden z nich wymaga przyciecia
        if (p1.z<zLimit || p2.z<zLimit){
            //ustawiamy zeby zawsze ten sam punkt byl do przyciecia
            if (p1.z>zLimit) std::swap(p1, p2); //teraz zawsze p1 to ten niepoprawny
            double clipFactor = (zLimit - p1.z) / (p2.z - p1.z);
            //wyznaczamy nowy punkt na granicy zLimit
            p1.x = p1.x+clipFactor*(p2.x - p1.x);
            p1.y = p1.y+clipFactor*(p2.y - p1.y);
            p1.z = zLimit;
        }
        return true;
    }


    //rzutowanie wzgledem osi 0, 0, 0
    Point2D project3DTo2D(const Point3D &point, const double focal) {
        if (point.z <= 1e-4) return {0, 0};//zabezpiecznie przed dzieleniem przez 0
        //wyznaczamy rzut punktu 3D na 2D
        return {(point.x * focal) / point.z, (point.y * focal) / point.z};
    }


    double applyPrecision(const double value, const double precision) {
        if (precision <= 0) return value;
        return std::round(value / precision) * precision;
    };

    void normalizeVector(Point3D &vector) {
        double vectorLen = std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
        if (vectorLen > 0) {
            vector.x /= vectorLen;
            vector.y /= vectorLen;
            vector.z /= vectorLen;
        }
    }

    void transformPointToPointInCameraLocalAxes(Point3D &point, const Point3D &cameraPosition, const LocalAxes &cameraAxes) {
        Point3D relativePoint = {
            point.x - cameraPosition.x,
            point.y - cameraPosition.y,
            point.z - cameraPosition.z
        };

        //rzutujemy pozycje punktu wzgledem ustawienia kamery (mamy iloczyn skalarny relativePoint, czyli pozycji punktu wzgledem pozycji kamery, i ustawienia osi kamery)
        double localX = relativePoint.x * cameraAxes.right.x + relativePoint.y * cameraAxes.right.y + relativePoint.z *
                        cameraAxes.right.z;
        double localY = relativePoint.x * cameraAxes.up.x + relativePoint.y * cameraAxes.up.y + relativePoint.z *
                        cameraAxes.up.z;
        double localZ = relativePoint.x * cameraAxes.forward.x + relativePoint.y * cameraAxes.forward.y + relativePoint.z * cameraAxes.forward.z;

        point.x = localX;
        point.y = localY;
        point.z = localZ;
    };
}
