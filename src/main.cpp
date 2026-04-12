#include <SFML/Graphics.hpp>

#include "file-reader/ObjectReader.h"
#include "graphics/Projector.h"
#include <unistd.h>

const std::string helpString = std::format("--- Usage ---\n"
                                    "-f ./filename.txt   -> path to figure file to render (required)\n"
                                    "-m moveSpeed        -> camera movement speed (default 3)\n"
                                    "-r rotationSpeed    -> camera rotation speed (default 30)\n"
                                    "-h                  -> help\n"
                                    "-------------\n");

int main(int argc, char *argv[]) {
    std::string filename = "";
    double moveSpeed = 3;
    double rotationSpeed = 30;

    auto stringToDouble = [](std::string str) {
        try {
            return std::stod(str);
        } catch (const std::exception &e) {
            std::cerr << format("Invalid argument. Could not parse {} to double", str) << std::endl;
            exit(1);
        }
    };




    int opt;
    while ((opt = getopt(argc, argv, "f:m:r:h")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'm':
                moveSpeed = stringToDouble(optarg);
                break;
            case 'r':
                rotationSpeed = stringToDouble(optarg);
                break;
            case 'h':
                std::cout << helpString << std::endl;
                exit(0);
            default:
                std::cout << helpString << std::endl;
                exit(0);


        }
    }

    if (filename.empty()) {
        std::cerr << "Filename is required. (-f flag)" << std::endl;
        std::cout << helpString << std::endl;
        std::exit(1);
    }


    const auto figures = ObjectReader::readFiguresFromFile(filename);

    const Camera camera(Point3D{0, 0, 0}, 200, moveSpeed, rotationSpeed, 0.1, -1, 0.0001);

    Projector projector(800, 600, camera);

    projector.render(figures);

    return 0;
}
