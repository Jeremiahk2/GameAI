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

    // std::ifstream infile("input.txt");
    // std::string lineString;
    // infile >> lineString;
    // infile >> lineString;
    
    // //Loop through each edge.
    // while (lineString != std::string("ArrowLine1PointAx,ArrowLine1PointAy,ArrowLine1PointBx,ArrowLine1PointBy,ArrowLine2PointAx,ArrowLine2PointAy,ArrowLine2PointBx,ArrowLine2PointBy")) {
    //     std::stringstream data(lineString);
    //     //Set up vertex fields.
    //     std::string valueOne;
    //     std::string valueTwo;
    //     std::string weight;
    //     std::shared_ptr<Edge::Vertex> start(new Edge::Vertex);
    //     std::shared_ptr<Edge::Vertex> end(new Edge::Vertex);
    //     //Get input and put it into our vertices.
    //     getline(data, valueOne, ',');
    //     getline(data, valueTwo, ',');
    //     start->position = sf::Vector2f(stoi(valueOne), stoi(valueTwo));
    //     getline(data, valueOne, ',');
    //     getline(data, valueTwo, ',');
    //     end->position = sf::Vector2f(stoi(valueOne), stoi(valueTwo));
    //     getline(data, weight, ',');
    //     //Create edge.
    //     std::shared_ptr<Edge> edge(new Edge);
    //     edge->start = start;
    //     edge->end = end;
    //     edge->weight = stof(weight);
    //     //Add edge to graph
    //     graph.addEdge(edge);
    //     infile >> lineString;
    // }

    std::ifstream infile("test-files/large.txt");
    std::string lineString;
    infile >> lineString;
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
        graph.vertices.push_back(vertex);

        infile >> lineString;
    }
    infile >> lineString;
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
        edge->start = graph.vertices[stoi(valueTwo)];
        edge->end = graph.vertices[stoi(valueTwo)];
        edge->weight = stof(weight);
        //Add edge to graph
        graph.addEdge(edge);
        infile >> lineString;
        count++;
    }


    std::cout << graph.vertices.size() << std::endl;
    std::cout << graph.edges.size() << std::endl;

    // Pathfinding finder;
    // std::deque<std::shared_ptr<Edge::Vertex>> result = finder.calculateDijkstra(graph, graph.vertices[0], graph.vertices[20]);

    // for (int i = 0; i < result.size(); i++) {
    //     std::cout << result[i]->toString() << std::endl;
    // }

    return EXIT_SUCCESS;
}