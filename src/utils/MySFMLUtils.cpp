//
// Created by Bcom_ on 11.04.2026.
//

#include "MySFMLUtils.h"
#include <cmath>
#include "3DSpaceUtils.h"

namespace utils {
    Line2DWithThickness createLine2DWithThicknessFromLine(const Line2D &line, const double minLineThickness, const double maxLineThickness, const double focal) {

        Line2DWithThickness thickLine;
        thickLine.startZDepth = line.startZDepth;
        thickLine.endZDepth = line.endZDepth;

        const double baseWorldThickness = 0.1;

        double startThickness = (baseWorldThickness * focal) / std::max(0.001, line.startZDepth);
        double endThickness   = (baseWorldThickness * focal) / std::max(0.001, line.endZDepth);
        
        startThickness = std::clamp(startThickness, minLineThickness, maxLineThickness);
        endThickness   = std::clamp(endThickness,   minLineThickness, maxLineThickness);



        //wektor kierunkowy
        double dx = line.end.x - line.start.x;
        double dy = line.end.y - line.start.y;
        double length = std::sqrt(dx * dx + dy * dy);
        //zabezpieczenie przed dzieleniem przez bliskie 0, kiedy linia jest po prostu punktem
        if (length < 1e-6) {
            thickLine.startL = thickLine.startR = line.start;
            thickLine.endL = thickLine.endR = line.end;

            return thickLine;
        }

        //wyznacznie wektora prostopadlego, wedlug ktorego okreslimy punkty grubosci
        double nx = -dy / length;
        double ny = dx / length;

        //wyznaczamy punkty wokol prostej, zeby zasymulowac grubsza linie poprzez TriangleStrip
        const double startHalf = startThickness / 2.0;
        thickLine.startL = { line.start.x + nx * startHalf, line.start.y + ny * startHalf };
        thickLine.startR = { line.start.x - nx * startHalf, line.start.y - ny * startHalf };
        const double endHalf = endThickness / 2.0;
        thickLine.endL = { line.end.x + nx * endHalf, line.end.y + ny * endHalf };
        thickLine.endR = { line.end.x - nx * endHalf, line.end.y - ny * endHalf };

        return thickLine;

    }
}

