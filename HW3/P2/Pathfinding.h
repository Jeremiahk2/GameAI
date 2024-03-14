#pragma once
#include "Boid.h"
#include <list>
#include <iostream>
#include <memory>


class Edge {
    public:
    class Vertex {
        public:
        std::vector<std::shared_ptr<Edge>> outgoingEdges;

        sf::Vector2f position; //Can be X,Y coordinates, or for non-euclidean, can be an ID, like 1,1 for an ID of 1.

        Vertex(sf::Vector2f position);
        Vertex();

        bool equals(Vertex v);

    };
    std::shared_ptr<Edge::Vertex> start;
    std::shared_ptr<Edge::Vertex> end;

    float weight;

    bool equals(Edge e);

    std::string toString();
};

class Graph {
    public:
    std::vector<std::shared_ptr<Edge>> edges;

    std::vector<std::shared_ptr<Edge::Vertex>> vertices;

    std::shared_ptr<Edge::Vertex> addVertex(std::shared_ptr<Edge::Vertex> v);

    void addEdge(std::shared_ptr<Edge> e);
};


class Pathfinding {

    void calculateDijkstra(Graph graph, Edge::Vertex source, Edge::Vertex goal);
};