#include "Pathfinding.h"

std::shared_ptr<Edge::Vertex> Graph::addVertex(std::shared_ptr<Edge::Vertex> v) {
    bool duplicate = false;
    //Check for duplicate vertex.
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i]->equals(*v)) {
            return vertices[i];
        }
    }
    //Don't add a duplicate vertex!
    vertices.push_back(v);
    return vertices.back();
}

void Graph::addEdge(std::shared_ptr<Edge> e) {
    
    e->start = addVertex(e->start);
    e->end = addVertex(e->end);
    e->start->outgoingEdges.push_back(e);
    edges.push_back(e);
}

bool Edge::Vertex::equals(Edge::Vertex v) {
    if (position.x == v.position.x && position.y == v.position.y) {
        return true;
    }
    return false;
}
Edge::Vertex::Vertex(sf::Vector2f position) {
    this->position = position;
}

Edge::Vertex::Vertex() {
    this->position = sf::Vector2f(0.f, 0.f);
}

bool Edge::equals(Edge e) {
    if (start->equals(*e.start) && end->equals(*e.end) && weight == e.weight) {
        return true;
    }
    return false;
}

std::string Edge::toString() {
    std::string rtnString;
    rtnString += std::to_string((int)start->position.x) + "," + std::to_string((int)start->position.y) + "," +
        std::to_string((int)end->position.x) + "," + std::to_string((int)end->position.y) + "," + std::to_string(weight);
    return rtnString;
}

void Pathfinding::calculateDijkstra(Graph graph, Edge::Vertex source, Edge::Vertex goal) {
    std::deque<float> distances;
    std::deque<Edge::Vertex> prev;
    std::deque<Edge::Vertex> unvisited;
    for (int i = 0; i < graph.vertices.size(); i++) {
        if (source.equals(graph.vertices[i])) {
            distances.push_back(0.f);
        }
        else {
            distances.push_back(-1.f);
        }
        prev.push_back(Edge::Vertex(sf::Vector2f(-1.f, -1.f)));
        unvisited.push_back(graph.vertices[i]);
    }

    while (!unvisited.empty()) {

    }
}