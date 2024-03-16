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

std::string Edge::Vertex::toString() {
    std::string rtnString;
    rtnString += std::to_string((int)position.x) + "," + std::to_string((int)position.y);
    return rtnString;
}

bool Pathfinding::contains(std::deque<std::shared_ptr<Edge::Vertex>> vertices, std::shared_ptr<Edge::Vertex> vertex) {
    for (int i = 0; i < vertices.size(); i++) {
        if (vertices[i]->equals(*vertex)) {
            return true;
        }
    }
    return false;
}

int Pathfinding::findIndex(Graph graph, std::shared_ptr<Edge> edge) {
    for (int i = 0; i < graph.edges.size(); i++) {
        if (graph.edges[i]->equals(*edge)) {
            return i;
        }
    }
    return -1;
}
int Pathfinding::findIndex(Graph graph, std::shared_ptr<Edge::Vertex> vertex) {
    for (int i = 0; i < graph.vertices.size(); i++) {
        if (graph.vertices[i]->equals(*vertex)) {
            return i;
        }
    }
    return -1;
}

void Pathfinding::eraseVertex(Graph graph, std::shared_ptr<Edge::Vertex> vertex) {
    for (int i = 0; i < unvisited.size(); i++) {
        if (unvisited[i]->equals(*vertex)) {
            unvisited.erase(unvisited.begin() + i);
            return;
        }
    }
}

std::deque<std::shared_ptr<Edge::Vertex>> Pathfinding::calculateDijkstra(Graph graph, std::shared_ptr<Edge::Vertex>source, std::shared_ptr<Edge::Vertex>goal ) {
    std::deque<std::shared_ptr<Edge::Vertex>> s;
    std::map<int, std::multimap<float, int>::iterator> indices;
    //Initialize data structures to infinity/undefined/initial amount.
    for (int i = 0; i < graph.vertices.size(); i++) {
        prev.push_back(NULL);
        unvisited.push_back(graph.vertices[i]);
        indices.insert({i, distances.insert({FLT_MAX, i})});

    }
    //The distance to the source point is zero.
    distances.erase(indices.at(source->id));
    indices.insert_or_assign(source->id, distances.insert({0.f, source->id}));

    int numVisited = 0;

    while (numVisited < graph.vertices.size()) {
        //The current vertex.
        std::shared_ptr<Edge::Vertex> u;
        float min = FLT_MAX;
        //Loop through the distances array and find the vertex with the smallest distance that is unvisited. This is our current vertex. First one should be source.
        for (auto it = distances.begin(); it != distances.end();) {
            if (it->first <= min && !graph.vertices[it->second]->visited) {
                min = it->first;
                u = graph.vertices[it->second];
                distances.erase(it);
                it = distances.end();
            }
            else {
                ++it;
            }
        }
        //If we've reached the goal.
        if (u->equals(*goal)) {
            //If the goal equals the source, or if prev for the vertex is not empty.
            if (prev[u->id] != NULL || u->equals(*source)) {
                //Go back through the prev array to find our path, add it to the sequence.
                while (u != NULL) {
                    s.push_front(u);
                    u = prev[u->id];
                }
            }
            //Return the sequence regardless.
            return s;
        }
        std::cout << numVisited << std::endl;
        //Erase the vertex from the unvisited list.
        graph.vertices[u->id]->visited = true;
        numVisited++;
        //Loop through the current vertex's outgoing edges.
        for (int i = 0; i < u->outgoingEdges.size(); i++) {
            //Current opposite vertex (through and edge)
            std::shared_ptr<Edge::Vertex> v = u->outgoingEdges[i]->end;
            if (!v->visited) {
                // Get the accumulated distance to this vertex, plus the new edge weight to the neighboring vertex.
                float alt = min + u->outgoingEdges[i]->weight;
                // float alt = distances[u->id] + u->outgoingEdges[i]->weight;
                //If it's shorter than what is written down, replace it and update prev as well.
                if (alt < indices.at(v->id)->first) {
                    distances.erase(indices.at(v->id));
                    indices.insert_or_assign(v->id, distances.insert({alt, v->id}));
                    prev[v->id] = u;
                }
            }
        }
    }
    return s;
}

std::deque<std::shared_ptr<Edge::Vertex>> Pathfinding::calculateAStar(Graph graph, std::shared_ptr<Edge::Vertex> source, std::shared_ptr<Edge::Vertex> goal) {
    std::deque<std::shared_ptr<Edge::Vertex>> s;
    // //Initialize data structures to infinity/undefined/initial amount.
    // for (int i = 0; i < graph.vertices.size(); i++) {
    //     distances.push_back(FLT_MAX);
    //     prev.push_back(NULL);
    //     unvisited.push_back(graph.vertices[i]);
    // }
    // //The distance to the source point is zero.
    // distances[findIndex(graph, source)] = 0.f;

    // while (!unvisited.empty()) {
    //     //The current vertex.
    //     std::shared_ptr<Edge::Vertex> u;
    //     float min = FLT_MAX;
    //     //Loop through the distances array and find the vertex with the smallest distance that is unvisited. This is our current vertex. First one should be source.
    //     for (int i = 0; i < distances.size(); i++) {
    //         if (distances[i] <= min && contains(unvisited, graph.vertices[i])) {
    //             min = distances[i];
    //             u = graph.vertices[i];
    //         }
    //     }
    //     //If we've reached the goal.
    //     if (u->equals(*goal)) {
    //         //If the goal equals the source, or if prev for the vertex is not empty.
    //         if (prev[findIndex(graph, u)] != NULL || u->equals(*source)) {
    //             //Go back through the prev array to find our path, add it to the sequence.
    //             while (u != NULL) {
    //                 s.push_front(u);
    //                 u = prev[findIndex(graph, u)];
    //             }
    //         }
    //         //Return the sequence regardless.
    //         return s;
    //     }
    //     //Erase the vertex from the unvisited list.
    //     eraseVertex(graph, u);
    //     //Loop through the current vertex's outgoing edges.
    //     for (int i = 0; i < u->outgoingEdges.size(); i++) {
    //         //Current opposite vertex (through and edge)
    //         std::shared_ptr<Edge::Vertex> v = u->outgoingEdges[i]->end;
    //         // Get the accumulated distance to this vertex, plus the new edge weight to the neighboring vertex.
    //         float alt = distances[findIndex(graph, u)] + u->outgoingEdges[i]->weight;
    //         //If it's shorter than what is written down, replace it and update prev as well.
    //         if (alt < distances[findIndex(graph, v)]) {
    //             distances[findIndex(graph, v)] = alt;
    //             prev[findIndex(graph, v)] = u;
    //         }
    //     }
    // }
    return s;
}