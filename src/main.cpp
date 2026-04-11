#include <SFML/Graphics.hpp>

#include "file-reader/ObjectReader.h"
#include "graphics/Projector.h"


int main() {
    // auto figures = ObjectReader::readFiguresFromFile("../resources/szescian.txt");
    // auto figures = ObjectReader::readFiguresFromFile("../resources/szescian_4_x_4.txt");
    auto figures = ObjectReader::readFiguresFromFile("../resources/duzo_pudelek.txt");

    const Camera camera(Point3D{0, 0, 0}, 200, 0.1, -1);

    Projector projector(800, 600, camera);

    projector.render(figures);

    return 0;
}
