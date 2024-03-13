#pragma once
#include "Boid.h"
#include <list>


class Edge {
    public:
    class Vertex {
        public:
        std::vector<Edge> outgoingEdges;

        sf::Vector2f position; //Can be X,Y coordinates, or for non-euclidean, can be an ID, like 1,1 for an ID of 1.

        bool equals(Vertex v);

    };
    Vertex start;
    Vertex end;

    float weight;

    bool equals(Edge e);

    std::string toString();
};

class Graph {
    public:
    std::vector<Edge> edges;

    std::vector<Edge::Vertex> vertices;

    void addVertex(Edge::Vertex v);

    void addEdge(Edge e);
};


class Pathfinding {

};