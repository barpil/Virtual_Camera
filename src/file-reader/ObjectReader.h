//
// Created by Bcom_ on 04.04.2026.
//

#ifndef WIRTUALNA_KAMERA_FILEREADER_H
#define WIRTUALNA_KAMERA_FILEREADER_H
#include <string>
#include <utility>

#include "../graphics/Figure.h"


class ObjectReader {
    std::string fileName;
public:
    static std::vector<Figure> readFiguresFromFile(std::string fileName);

private:
    static Figure createFigureFromString(std::string figureString);
};


#endif //WIRTUALNA_KAMERA_FILEREADER_H
