#include <SFML/Graphics.hpp>

#include "file-reader/ObjectReader.h"
#include "graphics/Projector.h"

int main() {
    auto figures = ObjectReader::readFiguresFromFile("../settings/bryly.txt");

    Projector projector(800, 600, 400, Point{-3, 0, -3});

    for (Figure const &figure : figures) {
        projector.drawFigure(figure);
    }

    projector.render();

    return 0;
}
