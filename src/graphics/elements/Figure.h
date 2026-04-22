//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_FIGURE_H
#define WIRTUALNA_KAMERA_FIGURE_H
#include <format>
#include <vector>
#include <iostream>
#include <SFML/Graphics/Color.hpp>
#include <format>
#include "Point.h"


struct Edge {
    int start;
    int end;
};

struct Wall {
    std::vector<int> vertexIdxs;
};

class Figure {
public:
    std::vector<Point3D> points;
    std::vector<Edge> edges;
    sf::Color color;
    std::vector<Wall> walls;

    explicit Figure(const std::vector<Point3D> &points, const std::vector<Edge> &edges, const sf::Color &color, const std::vector<Wall> &walls)
        : points(points),
          edges(edges),
          color(color),
          walls(walls){
    }

    friend std::ostream &operator<<(std::ostream &os, const Figure &f) {
        os << std::format("Color: ({}, {}, {}) ", f.color.r, f.color.g, f.color.b);
        os << "| Points: ";
        for (auto const &[x, y, z]: f.points) {
            os << "(x: " << x << ", y: " << y << ", z: " << z << ") ";
        }
        os << "| Edges: ";
        for (auto [start, end]: f.edges) {
            os << "(" << start << "->" << end << ") ";
        }
        os << "| Walls: ";
        for (auto const& wall: f.walls) {
            os << "(";
            for (int v: wall.vertexIdxs) {
                os << v << ", ";
            }
            os << "\b\b) ";
        }
        return os;
    }
};


#endif //WIRTUALNA_KAMERA_FIGURE_H
