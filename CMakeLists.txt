cmake_minimum_required(VERSION 4.2)
project(Wirtualna_kamera)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML CONFIG REQUIRED)

add_executable(Wirtualna_kamera src/main.cpp
        src/file-reader/ObjectReader.cpp
        src/file-reader/ObjectReader.h
        src/graphics/Projector.cpp
        src/graphics/Projector.h
        src/utils/3DSpaceUtils.cpp
        src/utils/3DSpaceUtils.h
        src/graphics/elements/Camera.cpp
        src/graphics/elements/Camera.h
        src/graphics/elements/Figure.cpp
        src/graphics/elements/Figure.h
        src/graphics/elements/Point.h
        src/graphics/binaries/RobotoMonoFont.h
)

target_link_libraries(Wirtualna_kamera PRIVATE SFML::Graphics SFML::Window SFML::System)