#include "GameState.h"

int64_t GameState::delta = 0;
Boid GameState::character;
Graph GameState::graph;
std::deque<std::shared_ptr<Edge::Vertex>> GameState::fillers;
int GameState::tileSize = 0;