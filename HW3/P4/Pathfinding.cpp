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

std::deque<std::shared_ptr<Edge::Vertex>> Pathfinding::calculateDijkstra(Graph graph, std::shared_ptr<Edge::Vertex>source, std::shared_ptr<Edge::Vertex>goal ) {
    std::deque<std::shared_ptr<Edge::Vertex>> s;
    std::map<int, std::multimap<float, int>::iterator> indices;
    //Initialize data structures to infinity/undefined/initial amount.
    for (int i = 0; i < graph.vertices.size(); i++) {
        prev.push_back(NULL);
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

float Pathfinding::euclidean(std::shared_ptr<Edge::Vertex> source, std::shared_ptr<Edge::Vertex> goal) {
    return sqrt(pow((source->position - goal->position).x, 2) + pow((source->position - goal->position).y, 2));
}

float Pathfinding::manhattan(std::shared_ptr<Edge::Vertex> source, std::shared_ptr<Edge::Vertex> goal) {
    sf::Vector2f path = source->position - goal->position;
    return abs(path.x) + abs(path.y);
}

std::deque<std::shared_ptr<Edge::Vertex>> Pathfinding::calculateAStar(Graph graph, std::shared_ptr<Edge::Vertex> source, std::shared_ptr<Edge::Vertex> goal) {
    std::deque<std::shared_ptr<Edge::Vertex>> s;
    std::map<int, std::multimap<float, std::pair<float, int>>::iterator> heuristicIndices;
    //Initialize data structures to infinity/undefined/initial amount.
    for (int i = 0; i < graph.vertices.size(); i++) {
        prev.push_back(NULL);
        heuristicIndices.insert({i, heuristics.insert({FLT_MAX, {FLT_MAX, i}})});
    }
    //The distance to the source point is zero.
    heuristics.erase(heuristicIndices.at(source->id));
    heuristicIndices.insert_or_assign(source->id, heuristics.insert({euclidean(source, goal), {0.f, source->id}}));
    
    int numVisited = 0;

    while (numVisited < graph.vertices.size()) {
        //The current vertex.
        std::shared_ptr<Edge::Vertex> u;
        float min = FLT_MAX;
        float minDistance = FLT_MAX;
        //Loop through the distances array and find the vertex with the smallest distance that is unvisited. This is our current vertex. First one should be source.
        for (auto it = heuristics.begin(); it != heuristics.end();) {
            //Realistically this loop should only happen once since the first value should be a minimum, unvisited node. But with inadmissable it might not be.
            if (it->first <= min && !graph.vertices[it->second.second]->visited) {
                min = it->first;
                minDistance = it->second.first;
                u = graph.vertices[it->second.second];
                heuristics.erase(it);
                it = heuristics.end();
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
        //Erase the vertex from the unvisited list.
        graph.vertices[u->id]->visited = true;
        numVisited++;
        //Loop through the current vertex's outgoing edges.
        for (int i = 0; i < u->outgoingEdges.size(); i++) {
            //Current opposite vertex (through and edge)
            std::shared_ptr<Edge::Vertex> v = u->outgoingEdges[i]->end;
            if (!v->visited) {
                // Get the accumulated distance to this vertex, plus the new edge weight to the neighboring vertex.
                float alt = minDistance + u->outgoingEdges[i]->weight;
                //If it's shorter than what is written down, replace it and update prev as well.
                if (alt < heuristicIndices.at(v->id)->second.first) {
                    heuristics.erase(heuristicIndices.at(v->id));
                    heuristicIndices.insert_or_assign(v->id, heuristics.insert({alt + euclidean(v, goal), {alt, v->id}}));
                    prev[v->id] = u;
                }
            }
        }
    }
    return s;
}