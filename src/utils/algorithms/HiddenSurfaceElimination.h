//
// Created by Bcom_ on 04.05.2026.
//

#ifndef WIRTUALNA_KAMERA_HIDDENSURFACEELIMINATION_H
#define WIRTUALNA_KAMERA_HIDDENSURFACEELIMINATION_H
#include <vector>
#include "../../graphics/elements/Point.h"
#include "BSPTree/BSPTree.h"

void initializeBSPTree(const std::vector<std::vector<Point3DWithColor>>& polygons);
BSPTree& getBSPTree();
std::vector<const std::vector<Point3DWithColor>*> getPolygonsInBackToFrontOrder(const Point3D &cameraPosition);


#endif //WIRTUALNA_KAMERA_HIDDENSURFACEELIMINATION_H
