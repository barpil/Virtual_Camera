//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_FIGURE_H
#define WIRTUALNA_KAMERA_FIGURE_H
#include <vector>
#include <iostream>

#include "Point.h"


struct Edge {
    int start;
    int end;
};

class Figure {
public:
    std::vector<Point3D> points;
    std::vector<Edge> edges;

    explicit Figure(const std::vector<Point3D> &points, const std::vector<Edge> &edges)
        : points(points),
          edges(edges) {
    }

    friend std::ostream& operator<<(std::ostream& os, const Figure& f) {
        os << "Points: ";
        for (auto const& [x, y, z]: f.points) {
            os << "(x: " << x << ", y: " << y << ", z: " << z << ") ";
        }
        os << "| Edges: ";
        for (auto [start, end]: f.edges) {
            os << "(" << start << "->" << end << ") ";
        }
        return os;
    }
};


#endif //WIRTUALNA_KAMERA_FIGURE_H
