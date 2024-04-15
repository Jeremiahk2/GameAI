#pragma once
#include "GameState.h"
#include "Pathfinding.h"
#include "SteeringBehavior.h"
#include <memory>

//Abstract DecisionTreeNode class.
class DecisionTreeNode {
    private:

    public:
    virtual std::shared_ptr<DecisionTreeNode> makeDecision() = 0;
};

//Abstract Action class.
class Action : public DecisionTreeNode {
    private:
    public:
    // std::shared_ptr<DecisionTreeNode> makeDecision() override;
};

class GoCenter : public Action {
    private:
    public:
    std::shared_ptr<DecisionTreeNode> makeDecision() override;
};

class WanderAction : public Action {
    private:
    public:
    std::shared_ptr<DecisionTreeNode> makeDecision() override;
};

class Decision : public DecisionTreeNode {
    private:
    public:
    std::shared_ptr<DecisionTreeNode> trueNode;
    std::shared_ptr<DecisionTreeNode> falseNode;
    // void *testValue();

    virtual std::shared_ptr<DecisionTreeNode> getBranch() = 0;

    virtual std::shared_ptr<DecisionTreeNode> makeDecision() = 0;
};

class NearWall : public Decision {
    private:
    public:

    std::shared_ptr<DecisionTreeNode> getBranch() override;

    std::shared_ptr<DecisionTreeNode> makeDecision() override;
};