#pragma once
//File for keeping track of the state of the game environment so that AI can make informed decisions.
#include <map>
#include "Pathfinding.h"
#include "Boid.h"

// class GameState {
//     private:
//     public:

//     //Knowledge: Immutable after game has started.
//     static Graph graph;
//     static std::deque<std::shared_ptr<Edge::Vertex>> fillers;
//     static int tileSize;
//     //State. Things that change throughout the game.
//     static Boid character;
//     static int64_t delta;
// };