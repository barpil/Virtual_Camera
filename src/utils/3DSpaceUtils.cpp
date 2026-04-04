//
// Created by Bcom_ on 04.04.2026.
//
#include "3DSpaceUtils.h"

#include <cmath>

#include "../graphics/Figure.h"

//Przeczytałem coś, że takie liczenie odleglosci w przestrzeni euklidesowej moze byc niepoprawne, bo powoduje
//efekt rybiego oka. Ale jeszcze tego nie sprawdzałem

namespace utils{
    double distance3D(const Point &p1, const Point &p2) {
        const double dx = p2.x-p1.x;
        const double dy = p2.y-p1.y;
        const double dz = p2.z-p1.z;

        return sqrt((dx*dx)+(dy*dy)+(dz*dz));
    }
}


