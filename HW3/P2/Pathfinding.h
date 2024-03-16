#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <cmath>
#include <list>
#include <iostream>
#include <memory>
#include <deque>
#include <limits>
#include <cfloat>


class Edge {
    public:
    class Vertex {
        public:
        std::vector<std::shared_ptr<Edge>> outgoingEdges;

        int id;

        sf::Vector2f position; //Can be X,Y coordinates, or for non-euclidean, can be an ID, like 1,1 for an ID of 1.

        Vertex(sf::Vector2f position);
        Vertex();

        bool equals(Vertex v);

        std::string toString();

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
public:
    std::deque<float> distances;
    std::deque<std::shared_ptr<Edge::Vertex>> prev;
    std::deque<std::shared_ptr<Edge::Vertex>> unvisited;

    int findIndex(Graph graph, std::shared_ptr<Edge> edge);

    int findIndex(Graph graph, std::shared_ptr<Edge::Vertex> vertex);

    void eraseEdge(Graph graph, std::shared_ptr<Edge> edge);
    void eraseVertex(Graph graph, std::shared_ptr<Edge::Vertex> vertex);

    bool contains(std::deque<std::shared_ptr<Edge::Vertex>> vertices, std::shared_ptr<Edge::Vertex> vertex);

    std::deque<std::shared_ptr<Edge::Vertex>> calculateDijkstra(Graph graph, std::shared_ptr<Edge::Vertex>source, std::shared_ptr<Edge::Vertex>goal);

    std::deque<std::shared_ptr<Edge::Vertex>> calculateAStar(Graph graph, std::shared_ptr<Edge::Vertex> source, std::shared_ptr<Edge::Vertex> goal);
};