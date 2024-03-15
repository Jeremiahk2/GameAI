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

bool isDuplicate(std::shared_ptr<Edge> edge) {
    for (int i = 0; i < edge->start->outgoingEdges.size(); i++) {
        if (edge->start->outgoingEdges[i]->equals(*edge)) {
            return true;
        }
    }
    return false;
}


int main() {

    srand(time(0));
    //Generate every possible vertex in a 1000 x 1000 grid.
    std::deque<std::shared_ptr<Edge::Vertex>> picks;
    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 1000; j++) {
            picks.push_back(std::shared_ptr<Edge::Vertex>(new Edge::Vertex(sf::Vector2f(i, j))));
        }
    }
    std::cout << "Picks Done" << std::endl;

    //Generate all of our possible indices into our picks list by using 10 lists of 100,000.
    std::array<std::deque<int>, 10> indices;
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 100000; j++) {
            indices.at(i).push_back(j + (i * 100000));
        }
    }
    std::cout << "Indices done" << std::endl;

    //Generate a random index into one of the ten index lists and add that vertex to vertices. Delete the index after to ensure no duplicates.
    std::deque<std::shared_ptr<Edge::Vertex>> vertices;
    int numNonzeroLists = 10;
    for (int i = 0; i < 100000; i++) {
        int outerIndex = rand() % numNonzeroLists;
        int innerIndex = rand() % indices.at(outerIndex).size();
        vertices.push_back(picks[indices.at(outerIndex).at(innerIndex)]);
        indices.at(outerIndex).erase(indices.at(outerIndex).begin() + innerIndex);
    }
    std::cout << "Vertices done" << std::endl;

    //Ensure connectivity by linking all edges in a master branch.
    std::deque<std::shared_ptr<Edge>> edges;
    for (int i = 0; i < vertices.size() - 1; i++) {
        float weight = sqrt(pow((vertices[i]->position - vertices[i+1]->position).x, 2) + pow((vertices[i]->position - vertices[i+1]->position).y, 2));
        std::shared_ptr<Edge> forward(new Edge);
        forward->start = vertices[i];
        forward->end = vertices[i+1];
        forward->weight = weight;
        vertices[i]->outgoingEdges.push_back(forward);

        std::shared_ptr<Edge> reverse(new Edge);
        reverse->start = vertices[i+1];
        reverse->end = vertices[i];
        reverse->weight = weight;
        vertices[i+1]->outgoingEdges.push_back(reverse);
        edges.push_back(forward);
        edges.push_back(reverse);
    }
    std::cout << "Master branch done" << std::endl;

    //Then randomly generate the remaining edges (No duplicates!)
    for (int i = 0; i < 250000; i++) {
        std::shared_ptr<Edge> edge(new Edge);
        edge->start = vertices[rand() % vertices.size()];
        edge->end = vertices[rand() % vertices.size()];
        while (edge->start->equals(*edge->end) || isDuplicate(edge)) {
            edge->end = vertices[rand() % vertices.size()];
        }
        edge->weight = sqrt(pow((edge->start->position - edge->end->position).x, 2) + pow((edge->start->position - edge->end->position).y, 2));
        edge->start->outgoingEdges.push_back(edge);
        edges.push_back(edge);
    }
    std::cout << "Edges done" << std::endl;

    std::ofstream output ("output.txt", std::ofstream::out);
    for (int i = 0; i < vertices.size(); i++) {
        output << vertices[i]->toString()<< std::endl;
    }
    std::cout << "Outputting vertices done" << std::endl;
    output << "Edges:" << std::endl;
    for (int i = 0; i < edges.size(); i++) {
        output << edges[i]->toString() << std::endl;
    }
    std::cout << "Outputting edges done" << std::endl;

    std::cout << "Final number of Vertices: " << vertices.size() << std::endl;
    std::cout << "Final number of edges: " << edges.size() << std::endl;

    return EXIT_SUCCESS;
}