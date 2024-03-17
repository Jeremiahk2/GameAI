#include "Timeline.h"
#include "SteeringBehavior.h"
#include "Pathfinding.h"

#include <cstdio>
#include <deque>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>
#include <sstream>

int main() {

    Graph smallGraph;
    
    std::ifstream sinFile("test-files/small.txt");
    std::string lineString;
    sinFile >> lineString;
    while (lineString != std::string("Edges:")) {
        std::stringstream data(lineString);
        std::string index;
        std::string x;
        std::string y;
        std::shared_ptr<Edge::Vertex> vertex(new Edge::Vertex);
        getline(data, index, ',');
        getline(data, x, ',');
        getline(data, y, ',');
        vertex->id = stoi(index);
        vertex->position = sf::Vector2f(stoi(x), stoi(y));
        smallGraph.vertices.push_back(vertex);

        sinFile >> lineString;
    }
    sinFile >> lineString;
    while (lineString != std::string("End")) {
        std::stringstream data(lineString);
        //Set up vertex fields.
        std::string valueOne;
        std::string valueTwo;
        std::string weight;
        //Get input and put it into our vertices.
        getline(data, valueOne, ',');
        getline(data, valueTwo, ',');
        getline(data, weight, ',');
        //Create edge.
        std::shared_ptr<Edge> edge(new Edge);
        edge->start = smallGraph.vertices[stoi(valueOne)];
        edge->end = smallGraph.vertices[stoi(valueTwo)];
        edge->weight = stof(weight);
        //Add edge to graph
        smallGraph.addEdge(edge);
        sinFile >> lineString;
    }



    Graph largeGraph;

    std::ifstream linFile("test-files/large.txt");
    linFile >> lineString;
    while (lineString != std::string("Edges:")) {
        std::stringstream data(lineString);
        std::string index;
        std::string x;
        std::string y;
        std::shared_ptr<Edge::Vertex> vertex(new Edge::Vertex);
        getline(data, index, ',');
        getline(data, x, ',');
        getline(data, y, ',');
        vertex->id = stoi(index);
        vertex->position = sf::Vector2f(stoi(x), stoi(y));
        largeGraph.vertices.push_back(vertex);

        linFile >> lineString;
    }
    linFile >> lineString;
    int count = 0;
    while (lineString != std::string("End")) {
        std::stringstream data(lineString);
        //Set up vertex fields.
        std::string valueOne;
        std::string valueTwo;
        std::string weight;
        //Get input and put it into our vertices.
        getline(data, valueOne, ',');
        getline(data, valueTwo, ',');
        getline(data, weight, ',');
        //Create edge.
        std::shared_ptr<Edge> edge(new Edge);
        edge->start = largeGraph.vertices[stoi(valueOne)];
        edge->end = largeGraph.vertices[stoi(valueTwo)];
        edge->weight = stof(weight);
        //Add edge to graph
        largeGraph.addEdge(edge);
        linFile >> lineString;
        count++;
    }

    // Pathfinding smallFinder;
    // std::deque<std::shared_ptr<Edge::Vertex>> smallResult = smallFinder.calculateAStar(smallGraph, smallGraph.vertices[0], smallGraph.vertices[31]);

    Pathfinding largeDijkstra;
    std::deque<std::shared_ptr<Edge::Vertex>> largeResult = largeDijkstra.calculateAStar(largeGraph, largeGraph.vertices[0], largeGraph.vertices[20]);




    std::cout << "Small Result: " << std::endl;
    for (int i = 0; i < smallResult.size(); i++) {
        std::cout << smallResult[i]->id << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Large Result: " << std::endl;
    for (int i = 0; i < largeResult.size(); i++) {
        std::cout << largeResult[i]->id << std::endl;
    }

    return EXIT_SUCCESS;
}