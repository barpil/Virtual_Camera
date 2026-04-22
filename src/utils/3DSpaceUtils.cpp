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

    Point3D calculateIntersectionPointWithZPlane(const Point3D& p1, const Point3D& p2, double zValue) {
        double clipFactor = (zValue - p1.z) / (p2.z - p1.z);
        //wyznaczamy nowy punkt na granicy zValue
        return {
            p1.x+clipFactor*(p2.x - p1.x),
            p1.y+clipFactor*(p2.y - p1.y),
            zValue
        };
    }


    //wystarczy clipping Z, bo SFML dokonuje automatycznie clippingu elementow, które wychodzą za view port
    //zwraca bool, czy warto rysowac te linie, czy jednak nie ma sensu
    bool performLineZClipping(Point3D &p1, Point3D &p2, const double zLimit) {
        bool p1IsVisible = p1.z >= zLimit;
        bool p2IsVisible = p2.z >= zLimit;
        //jezeli oba punkty sa za kamera to ich nie rysujemy
        if (!p1IsVisible && !p2IsVisible) return false;
        //jezeli jeden z nich wymaga przyciecia
        if (std::bit_xor<>()(p1IsVisible, p2IsVisible)){
            //ustawiamy zeby zawsze ten sam punkt byl do przyciecia
            if (p1.z>zLimit) std::swap(p1, p2); //teraz zawsze p1 to ten niepoprawny
            p1 = calculateIntersectionPointWithZPlane(p1, p2, zLimit);
        }
        return true;
    }

    std::optional<std::vector<Point3D>> performPolygonZClipping(std::vector<Point3D> &points, const double zLimit) {
        std::vector<Point3D> clippedPoints;

        for (int i = 0; i < points.size(); i++) {
            const Point3D& p1 = points[i];
            const Point3D& p2 = points[(i+1)%points.size()];

            const bool p1IsVisible = p1.z >= zLimit;
            const bool p2IsVisible = p2.z >= zLimit;

            if (p1IsVisible && p2IsVisible) {
                //jak oba widoczne to po prostu dodajemy nastepny
                clippedPoints.push_back(p2);
            } else if (p1IsVisible && !p2IsVisible) {
                //jezeli punkt koncowy wychodzi za ekram to dodajemy jego przeciecie z plaszczyzna Z
                clippedPoints.push_back(calculateIntersectionPointWithZPlane(p1, p2, zLimit));
            } else if (!p1IsVisible && p2IsVisible) {
                //jezeli punkt poczatkowy wychodzi za ekran, a koncowy nie to dodajemy przeciecie poczatku z plaszczyzna Z i punkt koncowy
                clippedPoints.push_back(calculateIntersectionPointWithZPlane(p1, p2, zLimit));
                clippedPoints.push_back(p2);
            }
            //jak oba poza to nic nie dodajemy
        }
        if (clippedPoints.empty()) return std::nullopt;
        return clippedPoints;
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
    }

    double calculateDepthFactor(const double z, const double minZ, const double maxZ, const double minFactor, const double maxFactor, const double factorExponent) {
        //mapujemy polozenie z na wspolczynnik glebokosci. minZ i maxZ okresla dla jakich wartosci z factor zaczyna przyjmowac minimalna/maksymalna wartosc
        double factor = (maxZ - z) / (maxZ - minZ);
        //przycinamy wartosci do <0, 1>
        factor = std::clamp(factor, 0.0, 1.0);
        //dostosowujemy sobie tempo i w ktorym kierunku exponent rosnie szybciej, a w ktorym wolniej (jak > 0 to szybciej maleje, jak < 0 to szybicej rosnie
        factor = std::pow(factor, factorExponent);
        //przemapowanie na zakres <minFactor, maxFactor>
        factor = std::lerp(minFactor, maxFactor, factor);
        return factor;
    }
}
