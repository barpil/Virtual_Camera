#include <SFML/Graphics.hpp>

#include "file-reader/ObjectReader.h"
#include "graphics/Projector.h"


int main() {
    auto figures = ObjectReader::readFiguresFromFile("../settings/szescian.txt");

    const Camera camera(Point3D{0, 0, -1}, Direction{Sign::POSITIVE, Sign::POSITIVE, Sign::POSITIVE}, 200);

    Projector projector(800, 600, camera);

    projector.render(figures);

    return 0;
}
