//
// Created by Bcom_ on 04.04.2026.
//

#include "ObjectReader.h"

#include <algorithm>
#include <format>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ranges>

std::vector<Figure> ObjectReader::readFiguresFromFile(std::string fileName) {
    std::filesystem::path fullPath = std::filesystem::current_path() / fileName;

    std::cout << "Trying to read from: " << fullPath << std::endl;

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << fullPath << std::endl;
        exit(1);
    }

    std::vector<Figure> figures;
    std::string line;

    int i = 0;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        Figure figure = createFigureFromString(line);
        figures.push_back(figure);
        i++;
        std::cout << "Figure " << i << ". read: " << figure << std::endl;
    }

    std::cout << "All (" << i << ") figures read correctly" << std::endl;

    file.close();

    return figures;
}

Figure ObjectReader::createFigureFromString(std::string figureString) {
    std::string originalString = figureString;
    erase_if(figureString, [](const unsigned char c) {
                 return std::isspace(c) || c == '{' || c == '}' || c == '(' || c == ')';
             }
    );


    std::optional<std::string> centerOffsetStringPart;
    std::string colorStringPart;
    std::string pointsStringPart;
    std::string edgesStringPart;

    std::stringstream ss(figureString);
    if (figureString.contains('/')) {
        centerOffsetStringPart.emplace();
        //bo inaczej jest nullptr i nie nie da sie poprawnie uzyc wskaznika, bo miejsce w pamieci nie istnieje
        std::getline(ss, *centerOffsetStringPart, '/'); //opcjonalny parametr do przesuniecia calej figury o offset
    }


    //Check for required figure definition elements
    if (!std::getline(ss, colorStringPart, '|') ||
        !std::getline(ss, pointsStringPart, '|') ||
        !std::getline(ss, edgesStringPart, '|')
    ) {
        std::cerr << "Invalid figure input: \"" << originalString << "\"" << std::endl;
        std::cerr <<
                "Correct input format: CenterOffset (optional) / Color | Points | Edges\n(where CenterOffset is an optional passed figure offset from coordinate center, Color is figure color in (uint;uint;uint) format, Points is comma separated list of points {x;y;z} and Edges is comma separated list of points to connect {start;end})"
                << std::endl
                << "Example:\n"
                << "(0;255;0) | {-1;-1;-1}, {1;-1;-1}, {1;1;-1}, {-1;1;-1} | {0;1}, {1;2}, {2;3}, {3;0}   ->  Green cube at center\n"
                << "OR\n"
                << "{0;1;0}  / (0;255;0) | {-1;-1;-1}, {1;-1;-1}, {1;1;-1}, {-1;1;-1} | {0;1}, {1;2}, {2;3}, {3;0}    ->  Green cube offset by {0;1;0}\n";
        exit(-1);
    }

    //OFFSET (if exists)
    Point3D offset = {0, 0, 0};
    if (centerOffsetStringPart.has_value()) {
        auto offsetVector = readPointsFromString(centerOffsetStringPart.value(), ',', ';', offset);
        if (offsetVector.size() != 1) {
            std::cerr << std::format("CenterOffset (if specified) must consist of 1 coordinate but passed {}: {}\n",
                                     offsetVector.size(), centerOffsetStringPart.value());
            exit(-1);
        }
        offset = offsetVector[0];
    }

    //COLOR
    auto colorVector = readPointsFromString(colorStringPart, ',', ';', {0, 0, 0});
    if (colorVector.size() != 1) {
        std::cerr << std::format("Color must consist of 1 coordinate but passed {}: {}\n", colorVector.size(),
                                 colorStringPart);
        exit(-1);
    }
    sf::Color color = {
        static_cast<uint8_t>(std::clamp(colorVector[0].x, 0.0, 255.0)),
        static_cast<uint8_t>(std::clamp(colorVector[0].y, 0.0, 255.0)),
        static_cast<uint8_t>(std::clamp(colorVector[0].z, 0.0, 255.0)
        )
    };


    //POINTS
    std::vector<Point3D> points = readPointsFromString(pointsStringPart, ',', ';', offset);

    //EDGES
    std::vector<std::string> edgeStrings;
    ss = std::stringstream(edgesStringPart);
    std::string edgeString;
    while (std::getline(ss, edgeString, ',')) {
        edgeStrings.push_back(edgeString);
    }

    std::vector<Edge> edges;

    for (const std::string &es: edgeStrings) {
        std::vector<int> pointIdxs;
        std::stringstream esStream(es);
        std::string value;

        while (std::getline(esStream, value, ';')) {
            try {
                pointIdxs.push_back(std::stoi(value));
            } catch (...) {
                std::cerr << "Non integer found in string : " << value << std::endl;
                exit(1);
            }
        }

        if (pointIdxs.size() != 2) {
            std::cerr <<
                    "Invalid edge sequence. Sequence should contain exactly 2 Point3D indexes (First Point3D is '1') {start;end}: "
                    << es << std::endl;
            continue;
        }

        Edge e{pointIdxs[0], pointIdxs[1]};
        edges.push_back(e);
    }

    return Figure(points, edges, color);
}

//wyciaga wektor obiektow Point3D ze stringa w formacie x1;y1;z1,x2;y2;z2 ...  (';' to przyklad pointsDelimiter, a ',' to coordinateDelimiter)
std::vector<Point3D> ObjectReader::readPointsFromString(const std::string &inputString, const char pointsDelimiter,
                                                        const char coordinateDelimiter, const Point3D &offset) {
    std::vector<std::string> pointStrings;
    auto ss = std::stringstream(inputString);
    std::string pointString;
    while (std::getline(ss, pointString, pointsDelimiter)) {
        pointStrings.push_back(pointString);
    }

    std::vector<Point3D> points;

    for (const std::string &ps: pointStrings) {
        std::vector<double> coords;
        std::stringstream psStream(ps);
        std::string value;

        while (std::getline(psStream, value, coordinateDelimiter)) {
            try {
                coords.push_back(std::stof(value));
            } catch (...) {
                std::cerr << "Non double found in string : " << value << std::endl;
                exit(1);
            }
        }

        if (coords.size() != 3) {
            std::cerr << std::format(
                "Invalid Point sequence. Sequence should contain exactly 3 coordinates x{}y{}z: {}",
                coordinateDelimiter, coordinateDelimiter, ps) << std::endl;
            continue;
        }


        //adding points with offset
        Point3D p{coords[0] + offset.x, coords[1] + offset.y, coords[2] + offset.z};
        points.push_back(p);
    }

    return points;
}
