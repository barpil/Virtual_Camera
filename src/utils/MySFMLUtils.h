//
// Created by Bcom_ on 11.04.2026.
//

#ifndef WIRTUALNA_KAMERA_MYSFMLUTILS_H
#define WIRTUALNA_KAMERA_MYSFMLUTILS_H

#include <SFML/Graphics.hpp>

#include "../graphics/elements/Point.h"


namespace utils {
    Line2DWithThickness createLine2DWithThicknessFromLine(const Line2D &line, double minLineThickness, double maxLineThickness, double focal);
}

#endif //WIRTUALNA_KAMERA_MYSFMLUTILS_H
