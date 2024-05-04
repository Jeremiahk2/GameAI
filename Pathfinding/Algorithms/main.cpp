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
    srand(time(0));
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

    linFile.close();
    sinFile.close();
    Timeline global;
    int tic = 1;
    Timeline frameTime(&global, tic);


    int numTests = 2000;
    int totalTime = 0;
    int totalFill = 0;
    int totalFringe = 0;
    std::vector<int>endpoints;
    for (int i = 0; i < numTests; i++) {
        endpoints.push_back((rand() % (largeGraph.vertices.size() - 1)) + 1);
    }

    std::vector<int>startpoints;
    for (int i = 0; i < numTests; i++) {
        startpoints.push_back((rand() % (largeGraph.vertices.size() - 1)) + 1);
    }

    totalTime = 0;
    totalFill = 0;
    totalFringe = 0;
    for (int i = 0; i < numTests; i++) {
        Pathfinding largeDijkstra;
        int startTime = frameTime.getTime();
        largeDijkstra.calculateDijkstra(largeGraph, largeGraph.vertices[startpoints[i]], largeGraph.vertices[endpoints[i]]);
        int elapsedTime = frameTime.getTime() - startTime;
        totalTime += elapsedTime;


        int numFill = 0; //Number of fill/closed set.
        for (int i = 0; i < largeGraph.vertices.size(); i++) {
            if (largeGraph.vertices[i]->visited) {
                numFill++;
            }
            largeGraph.vertices[i]->visited = false;
        }
        int numFringe = 0;
        for (auto it = largeDijkstra.distances.begin(); it != largeDijkstra.distances.end();) {
            if (it->first != FLT_MAX) {
                numFringe++;
                it++;
            }
            else {
                it = largeDijkstra.distances.end();
            }
        }
        totalFill += numFill;
        totalFringe += numFringe;
    }
    std::cout << "-Dijkstra stats-" << std::endl;
    std::cout << "Average time: " << totalTime / numTests << std::endl;
    std::cout << "Average Fill: " << totalFill / numTests << std::endl;
    std::cout << "Average Fringe: " << totalFringe / numTests << std::endl;


    totalTime = 0;
    totalFill = 0;
    totalFringe = 0;
    for (int i = 0; i < numTests; i++) {
        Pathfinding largeAstar;
        int startTime = frameTime.getTime();
        largeAstar.calculateAStar(largeGraph, largeGraph.vertices[startpoints[i]], largeGraph.vertices[endpoints[i]]);
        int elapsedTime = frameTime.getTime() - startTime;
        totalTime += elapsedTime;

        int numFill = 0;
        for (int i = 0; i < largeGraph.vertices.size(); i++) {
            if (largeGraph.vertices[i]->visited) {
                numFill++;
            }
            largeGraph.vertices[i]->visited = false;
        }
        int numFringe = 0;
        for (auto it = largeAstar.heuristics.begin(); it != largeAstar.heuristics.end();) {
            if (it->first != FLT_MAX) {
                numFringe++;
                it++;
            }
            else {
                it = largeAstar.heuristics.end();
            }
        }
        totalFill += numFill;
        totalFringe += numFringe;
    }

    std::cout << "-A* stats-" << std::endl;
    std::cout << "Average time: " << totalTime / numTests << std::endl;
    std::cout << "Average Fill: " << totalFill / numTests << std::endl;
    std::cout << "Average Fringe: " << totalFringe / numTests << std::endl;

    // Pathfinding smallAFinder;
    // Pathfinding smallDFinder;

    // int startTime = frameTime.getTime();
    // std::deque<std::shared_ptr<Edge::Vertex>> resultOne = smallAFinder.calculateAStar(smallGraph, smallGraph.vertices[0], smallGraph.vertices[5]);
    // // std::cout << frameTime.getTime() - startTime << std::endl;

    // for (int i = 0; i < smallGraph.vertices.size(); i++) {
    //     smallGraph.vertices[i]->visited = false;
    // }

    // startTime = frameTime.getTime();
    // std::deque<std::shared_ptr<Edge::Vertex>> resultTwo = smallDFinder.calculateDijkstra(smallGraph, smallGraph.vertices[0], smallGraph.vertices[5]);
    // // std::cout << frameTime.getTime() - startTime << std::endl;

    // std::cout << "Result One: " << std::endl;
    // for (int i = 0; i < resultOne.size(); i++) {
    //     std::cout << resultOne[i]->id << std::endl;
    // }
    // std::cout << std::endl;

    // std::cout << "Result Two: " << std::endl;
    // for (int i = 0; i < resultTwo.size(); i++) {
    //     std::cout << resultTwo[i]->id << std::endl;
    // }

    return EXIT_SUCCESS;
}