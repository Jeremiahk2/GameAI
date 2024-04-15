#include "DecisionTree.h"


//Decision making for the action: "GoCenter"
std::shared_ptr<DecisionTreeNode> GoCenter::makeDecision() {
    Pathfinding astar;
    std::deque<std::shared_ptr<Edge::Vertex>> path;
    Kinematic target;
    target.pos = sf::Vector2f(640.f / 2.f, 480.f / 2.f);
    int targetTileX = floor(target.pos.x / tileSize);
    int targetTileY = floor(target.pos.y / tileSize);
    std::shared_ptr<Edge::Vertex> targetVertex = GameState::fillers[targetTileX * verticalTiles + targetTileY];

    int boidTileX = floor(GameState::character.kinematic.pos.x / tileSize);
    int boidTileY = floor(GameState::character.kinematic.pos.y / tileSize);
    std::shared_ptr<Edge::Vertex> boidVertex = GameState::fillers[boidTileX * verticalTiles + boidTileY];


    path = astar.calculateAStar(GameState::graph, boidVertex, targetVertex);



    for (int i = 0; i < GameState::graph.vertices.size(); i++) {
        GameState::graph.vertices[i]->visited = false;
    }
    return std::shared_ptr<DecisionTreeNode>(this);
}

//Decision making for the action: "WanderAction"
std::shared_ptr<DecisionTreeNode> WanderAction::makeDecision() {
    Wander wander;
    Kinematic goal;
    wander.calculateAcceleration(GameState::character.steering, GameState::character.kinematic, goal);
    GameState::character.update(GameState::delta);
    return std::shared_ptr<DecisionTreeNode>(this);
}

//NearWall branch.
std::shared_ptr<DecisionTreeNode> NearWall::getBranch() {
    
    return trueNode;
}
//Decision making for the decision node: NearWall.
std::shared_ptr<DecisionTreeNode> NearWall::makeDecision() {

    return trueNode;
}