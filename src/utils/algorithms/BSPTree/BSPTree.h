//
// Created by Bcom_ on 04.05.2026.
//

#ifndef WIRTUALNA_KAMERA_BSPTREE_H
#define WIRTUALNA_KAMERA_BSPTREE_H
#include <memory>
#include <vector>
#include "../../../graphics/elements/Point.h"
#include <cmath>


struct Node {
    std::vector<std::vector<Point3DWithColor>> polygons;
    std::unique_ptr<Node> front;
    std::unique_ptr<Node> back;

    void addPolygon(const std::vector<Point3DWithColor> &polygon) {
        polygons.push_back(polygon);
    }

    std::vector<Point3DWithColor> getPolygon() {return polygons[0];}
};

enum Position {
    FRONT,
    BACK,
    SAME_PLANE,
    AMBIGUOUS
};

struct Plane {
    Point3D normal{};
    double d; // Z równania Ax + By + Cz + d = 0


    explicit Plane(const std::vector<Point3DWithColor> &polygon) {
        //Bo nie da sie plaszczyzny wyznaczyc dla odcinka lub punktu
        if (polygon.size() < 3) {return;}
        // Zakładamy, że wielokąt ma przynajmniej 3 punkty i nie są kolinearne
        Point3D v0 = {polygon[0].x, polygon[0].y, polygon[0].z};
        Point3D v1 = {polygon[1].x, polygon[1].y, polygon[1].z};
        Point3D v2 = {polygon[2].x, polygon[2].y, polygon[2].z};

        Point3D edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        Point3D edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};

        // Iloczyn wektorowy (Cross Product) dla normalnej
        normal.x = edge1.y * edge2.z - edge1.z * edge2.y;
        normal.y = edge1.z * edge2.x - edge1.x * edge2.z;
        normal.z = edge1.x * edge2.y - edge1.y * edge2.x;

        // Normalizacja
        double len = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (len > 0) {
            normal.x /= len; normal.y /= len; normal.z /= len;
        }

        // D = -(N dot P)
        d = -(normal.x * v0.x + normal.y * v0.y + normal.z * v0.z);
    }
};


class BSPTree {
    std::unique_ptr<Node> rootNode;
    int chooseBestRootElement(const std::vector<std::vector<Point3DWithColor>> &points);
    Position resolvePosition(const std::vector<Point3DWithColor> &resolvedElement, const std::vector<Point3DWithColor> &comparatorElement);
    void splitElement(const std::vector<Point3DWithColor>& element, const std::vector<Point3DWithColor>& comparator,
                        std::vector<Point3DWithColor>& frontPart, std::vector<Point3DWithColor>& backPart);
    void buildBSPTree(const std::vector<std::vector<Point3DWithColor>> &points);
    void addElementToTree(const std::vector<Point3DWithColor> &element);
    void addElementToTree(std::unique_ptr<Node>& node, const std::vector<Point3DWithColor> &element);

public:
    BSPTree(const std::vector<std::vector<Point3DWithColor>> &points);
    Node* getRootNode(){return rootNode.get();};
};



#endif //WIRTUALNA_KAMERA_BSPTREE_H
