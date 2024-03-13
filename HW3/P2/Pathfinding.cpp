#include "Pathfinding.h"

void Graph::addVertex(Edge::Vertex v) {
    bool duplicate = false;
    //Check for duplicate vertex.
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].equals(v)) {
            duplicate = true;
        }
    }
    //Don't add a duplicate vertex!
    if (!duplicate) {
        vertices.push_back(v);
    }
}

void Graph::addEdge(Edge e) {
    addVertex(e.start);
    addVertex(e.end);

    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i].equals(e.start)) {
            vertices[i].outgoingEdges.push_back(e);
        }
    }

    edges.push_back(e);



}

bool Edge::Vertex::equals(Edge::Vertex v) {
    if (position.x == v.position.x && position.y == v.position.y) {
        return true;
    }
    return false;
}

bool Edge::equals(Edge e) {
    if (start.equals(e.start) && end.equals(e.end) && weight == e.weight) {
        return true;
    }
    return false;
}

std::string Edge::toString() {
    std::string rtnString;
    rtnString += std::to_string((int)start.position.x) + "," + std::to_string((int)start.position.y) + "," +
        std::to_string((int)end.position.x) + "," + std::to_string((int)end.position.x) + "," + std::to_string(weight);
    return rtnString;
}