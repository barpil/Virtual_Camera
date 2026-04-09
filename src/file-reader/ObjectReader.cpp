//
// Created by Bcom_ on 04.04.2026.
//

#include "ObjectReader.h"

#include <algorithm>
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
        std::cout << "Figure " << i <<". read: " << figure << std::endl;
    }

    std::cout << "All (" << i <<") figures read correctly" << std::endl;

    file.close();

    return figures;
}

Figure ObjectReader::createFigureFromString(std::string figureString) {
    figureString.erase(
        std::ranges::remove_if(figureString, [](const unsigned char c) {
            return std::isspace(c) || c == '{' || c == '}';
        }).begin(),
        figureString.end()
    );

    std::string pointsStringPart;
    std::string edgesStringPart;

    std::stringstream ss(figureString);
    std::string excess;

    if (!(std::getline(ss, pointsStringPart, '|') &&
       std::getline(ss, edgesStringPart, '|') &&
       !std::getline(ss, excess, '|'))) {

        std::cerr << "Invalid figure input: \"" << figureString << "\"" << std::endl;
        std::cerr << "Correct input format: Points | Edges (where Point3D is comma separated list of points {x;y;z} and edge is comma separated list of points to connect {start;end})" <<std::endl;
        exit(-1);
       }

    // POINTS
    std::vector <std::string> pointStrings;
    ss = std::stringstream(pointsStringPart);
    std::string pointString;
    while (std::getline(ss, pointString, ',')) {
        pointStrings.push_back(pointString);
    }

    std::vector<Point3D> points;

    for (const std::string& ps : pointStrings) {
        std::vector<double> coords;
        std::stringstream psStream(ps);
        std::string value;

        while (std::getline(psStream, value, ';')) {
            try {
                coords.push_back(std::stof(value));
            } catch (...) {
                std::cerr << "Non integer found in string : " << value << std::endl;
                exit(1);
            }
        }

        if (coords.size() != 3) {
            std::cerr << "Invalid Point3D sequence. Sequence should contain exactly 3 coordinates {x;y;z}: " << ps << std::endl;
            continue;
        }

        Point3D p{coords[0], coords[1], coords[2]};
        points.push_back(p);
    }

    // EDGES
    std::vector <std::string> edgeStrings;
    ss = std::stringstream(edgesStringPart);
    std::string edgeString;
    while (std::getline(ss, edgeString, ',')) {
        edgeStrings.push_back(edgeString);
    }

    std::vector<Edge> edges;

    for (const std::string& es : edgeStrings) {
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
            std::cerr << "Invalid edge sequence. Sequence should contain exactly 2 Point3D indexes (First Point3D is '1') {start;end}: " << es << std::endl;
            continue;
        }

        Edge e{pointIdxs[0], pointIdxs[1]};
        edges.push_back(e);
    }

    return Figure(points, edges);
}


