#include <SFML/Graphics.hpp>

#include "file-reader/ObjectReader.h"
#include "graphics/Projector.h"


int main() {
    auto figures = ObjectReader::readFiguresFromFile("../resources/szescian.txt");

    const Camera camera(Point3D{0, 0, -1}, 200, 0.1, -1);

    Projector projector(800, 600, camera);

    projector.render(figures);

    return 0;
}
