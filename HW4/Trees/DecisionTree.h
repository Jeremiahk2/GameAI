#pragma once
#include "GameValue.h"
#include "Pathfinding.h"
#include "SteeringBehavior.h"
#include <memory>

//Abstract DecisionTreeNode class.
class DecisionTreeNode {
    private:

    public:

        static std::deque<std::string> actionQueue;

        virtual std::shared_ptr<DecisionTreeNode> makeDecision() = 0;
};

//Abstract Action class.
class Action : public DecisionTreeNode {
    private:
        std::string action;
    public:
        std::shared_ptr<DecisionTreeNode> makeDecision() override;

        Action(std::string action);
};

class Decision : public DecisionTreeNode {
    private:
        std::shared_ptr<DecisionTreeNode> getBranch();

    public:
        std::shared_ptr<GameValue> equivalence;
        std::shared_ptr<GameValue> upperBound;
        std::shared_ptr<GameValue> lowerBound;

        std::shared_ptr<GameValue> value;

        std::shared_ptr<DecisionTreeNode> trueNode;
        std::shared_ptr<DecisionTreeNode> falseNode;

        Decision();

        std::shared_ptr<DecisionTreeNode> makeDecision() override;
};