//
// Created by Bcom_ on 04.05.2026.
//

#include "BSPTree.h"

#include <cmath>

#include "../../GeneralUtils.h"
#include "../../../graphics/elements/Figure.h"


const double EPSILON = 0.000001f;

BSPTree::BSPTree(const std::vector<std::vector<Point3DWithColor>> &points) {
    buildBSPTree(points);
}

void BSPTree::buildBSPTree(const std::vector<std::vector<Point3DWithColor>> &points) {
    //Wybranie elementu jako root node'a i dodanie go jako rootNode drzewa
    int rootIdx = chooseBestRootElement(points);
    addElementToTree(points[rootIdx]);
    for (int i = 0; i < points.size(); i++) {
        if (i == rootIdx) continue;
        addElementToTree(points[i]);
    }
}

//Wybor jak najlepszego elementu zeby drzewo BSP bylo dobrze zbalansowane
//Zwraca index
int BSPTree::chooseBestRootElement(const std::vector<std::vector<Point3DWithColor>> &points) {
    const int CONSIDERED_CANDIDATES = 20;
    std::vector<int> polygonIdxs;
    for (int i=0; i<points.size(); i++) {
        if (points[i].size() >2) polygonIdxs.push_back(i);
    }
    const std::vector<int> candidates = polygonIdxs.size()-1 > CONSIDERED_CANDIDATES ? utils::getNRandomIntegersFromVector(polygonIdxs, CONSIDERED_CANDIDATES) : polygonIdxs;

    int bestCandidateIdx = candidates[0];
    //Im mniejszy score tym lepiej (score liczony a*(|frontCount-backCount|)+b*splitCount)
    double a = 0.2;
    double b = 0.8; //wyzsza waga dla liczby podzialow bo chcemy aby sciana centralna, byla jak najlepszym elementem podzialu
    double bestScore = std::numeric_limits<double>::infinity();

    for (int i = 0; i < candidates.size(); i++) {
        int frontCount = 0;
        int backCount = 0;
        int splitNeededCount = 0;
        for (int j = 0; j < candidates.size(); j++) {
            if (j==i) continue;
            switch (resolvePosition(points[candidates[j]], points[candidates[i]])) {
                case FRONT:
                    frontCount++;
                    break;
                case BACK:
                    backCount++;
                    break;
                case AMBIGUOUS:
                    splitNeededCount++;
                    break;
                default: ;
            }
        }
        int score = a * std::abs(frontCount - backCount) + b * splitNeededCount;
        if (score < bestScore) {
            bestScore = score;
            bestCandidateIdx = candidates[i];
        }
    }

    return bestCandidateIdx;
}



void BSPTree::addElementToTree(const std::vector<Point3DWithColor> &element) {
    addElementToTree(this->rootNode, element);
}


//Dodawanie elementow do wlasciwej galezi drzewa
void BSPTree::addElementToTree(std::unique_ptr<Node>& node, const std::vector<Point3DWithColor> &element) {
    if (node == nullptr) {
        node = std::make_unique<Node>();
        node->addPolygon(element);
    }else {
        switch (resolvePosition(element, node->getPolygon())) {
            case FRONT:
                if (node->front == nullptr) {
                    node->front = std::make_unique<Node>();
                    node->front->addPolygon(element);
                }else {
                    addElementToTree(node->front, element);
                }
                break;
            case BACK:
                if (node->back == nullptr) {
                    node->back = std::make_unique<Node>();
                    node->back->addPolygon(element);
                }else {
                    addElementToTree(node->back, element);
                }
                break;
            case SAME_PLANE:
                node->addPolygon(element);
                break;
            case AMBIGUOUS:
                std::vector<Point3DWithColor> frontPart;
                std::vector<Point3DWithColor> backPart;
                splitElement(element, node->getPolygon(), frontPart, backPart);
                if (node->front == nullptr) {
                    node->front = std::make_unique<Node>();
                    node->front->addPolygon(frontPart);
                }else {
                    addElementToTree(node->front, frontPart);
                }
                if (node->back == nullptr) {
                    node->back = std::make_unique<Node>();
                    node->back->addPolygon(backPart);
                }else {
                    addElementToTree(node->back, backPart);
                }
                break;
        }
    }
}

//Okreslenie pozycji resolvedElement wzgledem plaszczyzny w ktorej lezy comparatorElement (AMBIGUOUS znaczy, ze sie przecinaja)
Position BSPTree::resolvePosition(const std::vector<Point3DWithColor> &resolvedElement,
    const std::vector<Point3DWithColor> &comparatorElement) {
    if (resolvedElement.empty() || comparatorElement.size() < 3) return SAME_PLANE;

    const Plane plane(comparatorElement);

    int frontCount = 0;
    int backCount = 0;
    int onPlaneCount = 0;

    for (const auto& p : resolvedElement) {
        double dist = plane.normal.x * p.x + plane.normal.y * p.y + plane.normal.z * p.z + plane.d;

        if (dist > EPSILON) {
            frontCount++;
        } else if (dist < -EPSILON) {
            backCount++;
        } else {
            onPlaneCount++;
        }
    }

    if (frontCount > 0 && backCount > 0) return AMBIGUOUS;
    if (frontCount > 0) return FRONT;
    if (backCount > 0) return BACK;

    return SAME_PLANE;
}

//Podzial elementu na czesc przednia i tylna
void BSPTree::splitElement(const std::vector<Point3DWithColor>& splitElement,
                           const std::vector<Point3DWithColor>& comparatorElement,
                           std::vector<Point3DWithColor>& frontPart,
                           std::vector<Point3DWithColor>& backPart) {

    Plane plane(comparatorElement);

    for (size_t i = 0; i < splitElement.size(); ++i) {
        const auto& A = splitElement[i];
        const auto& B = splitElement[(i + 1) % splitElement.size()];

        double distA = plane.normal.x * A.x + plane.normal.y * A.y + plane.normal.z * A.z + plane.d;
        double distB = plane.normal.x * B.x + plane.normal.y * B.y + plane.normal.z * B.z + plane.d;

        if (distA >= -EPSILON) frontPart.push_back(A);
        if (distA <= EPSILON) backPart.push_back(A);

        //sprawdzenie czy krawedz przecina plaszczyzne
        if ((distA > EPSILON && distB < -EPSILON) || (distA < -EPSILON && distB > EPSILON)) {
            const double t = distA / (distA - distB);

            Point3DWithColor intersection;
            intersection.x = A.x + t * (B.x - A.x);
            intersection.y = A.y + t * (B.y - A.y);
            intersection.z = A.z + t * (B.z - A.z);


            intersection.color.r = A.color.r + t * (B.color.r - A.color.r);
            intersection.color.g = A.color.g + t * (B.color.g - A.color.g);
            intersection.color.b = A.color.b + t * (B.color.b - A.color.b);


            frontPart.push_back(intersection);
            backPart.push_back(intersection);
        }
    }
}



