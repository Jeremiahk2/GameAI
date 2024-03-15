#include "Pathfinding.h"
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <cstdio>
#include <deque>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>

float mapToRange(float theta) {
    theta = fmod(theta, 2 * M_PI);
    if (fabs(theta) <= M_PI) { //If theta is from -pi to pi
        return theta;
    }
    else if (theta > M_PI) { //if theta is greater than pi
        return theta - 2 * M_PI;
    }
    return theta + 2 * M_PI; //if theta is less than -pi
}


int main() {

    srand(time(0));

    std::deque<Edge::Vertex> picks;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            picks.push_back(Edge::Vertex(sf::Vector2f(i, j)));
        }
    }
    std::cout << "Picks Done" << std::endl;

    std::array<std::deque<int>, 10> indices;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 100000; j++) {
            indices.at(i).push_back(j + (i * 100000));
        }
    }
    std::cout << "Indices done" << std::endl;

    std::deque<Edge::Vertex> vertices;
    for (int i = 0; i < 100000; i++) {
        if (i % 1000 == 0) {
            // std::cout << "Vertices size: " << vertices.size() << std::endl;
            // std::cout << "Picks size: " << picks.size() << std::endl;
        }
        int outerIndex = rand() % 10;
        int innerIndex = rand() % indices.at(outerIndex).size();
        vertices.push_back(picks[indices.at(outerIndex).at(innerIndex)]);
        indices.at(outerIndex).erase(indices.at(outerIndex).begin() + innerIndex);

    }
    std::ofstream output ("output.txt", std::ofstream::out);
    for (int i = 0; i < vertices.size(); i++) {
        output << vertices[i].position.x << "," << vertices[i].position.y << std::endl;
    }

    return EXIT_SUCCESS;
}