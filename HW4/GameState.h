#pragma once
//File for keeping track of the state of the game environment so that AI can make informed decisions.
#include <map>
#include "Pathfinding.h"
#include "Boid.h"

class GameState {
    private:
    public:

    static Graph graph;

    static Boid character;

    static int64_t delta;
};