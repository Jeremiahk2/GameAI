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

    Graph graph;

    std::ifstream infile("input.txt");
    std::string lineString;
    infile >> lineString;
    infile >> lineString;
    
    //Loop through each edge.
    while (lineString != std::string("ArrowLine1PointAx,ArrowLine1PointAy,ArrowLine1PointBx,ArrowLine1PointBy,ArrowLine2PointAx,ArrowLine2PointAy,ArrowLine2PointBx,ArrowLine2PointBy")) {
        std::stringstream data(lineString);
        //Set up vertex fields.
        std::string valueOne;
        std::string valueTwo;
        std::string weight;
        std::shared_ptr<Edge::Vertex> start(new Edge::Vertex);
        std::shared_ptr<Edge::Vertex> end(new Edge::Vertex);
        //Get input and put it into our vertices.
        getline(data, valueOne, ',');
        getline(data, valueTwo, ',');
        start->position = sf::Vector2f(stoi(valueOne), stoi(valueTwo));
        getline(data, valueOne, ',');
        getline(data, valueTwo, ',');
        end->position = sf::Vector2f(stoi(valueOne), stoi(valueTwo));
        getline(data, weight, ',');
        //Create edge.
        std::shared_ptr<Edge> edge(new Edge);
        edge->start = start;
        edge->end = end;
        edge->weight = stof(weight);
        //Add edge to graph
        graph.addEdge(edge);
        infile >> lineString;
    }

    for (int i = 0; i < graph.vertices.size(); i++) {
        // std::cout << graph.vertices[i]->outgoingEdges.size() << std::endl;
        for (int j = 0; j < graph.vertices[i]->outgoingEdges.size(); j++) {
            std::cout << graph.vertices[i]->outgoingEdges[j]->toString() << std::endl;
        }
        std::cout << std::endl;
    }

    for (int i = 0; i < graph.edges.size(); i++) {
        std::cout << graph.edges[i]->toString() << std::endl;
    }

    return EXIT_SUCCESS;
}