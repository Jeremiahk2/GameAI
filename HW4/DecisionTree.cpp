#include "DecisionTree.h"


//Decision making for the action: "GoCenter"
std::shared_ptr<DecisionTreeNode> GoCenter::makeDecision() {

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