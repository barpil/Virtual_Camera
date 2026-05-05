//
// Created by Bcom_ on 04.05.2026.
//

#include <iostream>

#include "HiddenSurfaceElimination.h"


struct BspTreeCache {
    static std::unique_ptr<BSPTree> cache;

    static BSPTree& getTree() {
        if (!cache) {
            throw std::runtime_error("BSPTree not initialized");
        }
        return *cache;
    }

};
std::unique_ptr<BSPTree> BspTreeCache::cache = nullptr;


void initializeBSPTree(const std::vector<std::vector<Point3DWithColor>>& polygons) {
    BspTreeCache::cache = std::make_unique<BSPTree>(polygons);
    std::vector<const std::vector<Point3DWithColor>*> backToFrontPolygonOrder;
    std::vector<const std::vector<Point3DWithColor>*> frontToBackPolygonOrder;
}


void collectPolygonsRecursive(Node* node, const Point3D& cameraPos, std::vector<const std::vector<Point3DWithColor>*>& sortedList) {
    if (!node) return;

    Plane plane(node->getPolygon());
    double dist = plane.normal.x * cameraPos.x + plane.normal.y * cameraPos.y +
                 plane.normal.z * cameraPos.z + plane.d;
    //sprawdzamy polozenie kamery wzgledem plaszczyzny wezla
    if (dist > 0) {
        //kamera z przodu
        collectPolygonsRecursive(node->back.get(), cameraPos, sortedList);

        for (const auto& poly : node->polygons) {
            sortedList.push_back(&poly);
        }

        collectPolygonsRecursive(node->front.get(), cameraPos, sortedList);
    } else {
        //kamera z tylu
        collectPolygonsRecursive(node->front.get(), cameraPos, sortedList);

        for (const auto& poly : node->polygons) {
            sortedList.push_back(&poly);
        }

        collectPolygonsRecursive(node->back.get(), cameraPos, sortedList);
    }
}



std::vector<const std::vector<Point3DWithColor>*> getPolygonsInBackToFrontOrder(const Point3D &cameraPosition) {
    std::vector<const std::vector<Point3DWithColor>*> dynamicOrder;
    collectPolygonsRecursive(BspTreeCache::getTree().getRootNode(), cameraPosition, dynamicOrder);
    return dynamicOrder;
}




