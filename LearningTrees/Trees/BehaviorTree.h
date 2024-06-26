#pragma once
#include "Pathfinding.h"
#include "SteeringBehavior.h"
#include "GameValue.h"
#include <memory>

enum STATUS{SUCCESS, FAILURE, WAITING};

class BehaviorTreeNode {
    private:
    public:
        static std::map<std::string, enum STATUS> actionQueue;
        virtual enum STATUS run() = 0;
};

class Selector : public BehaviorTreeNode {
    private:

        int current = 0;
    public:

        std::deque<std::shared_ptr<BehaviorTreeNode>> children = std::deque<std::shared_ptr<BehaviorTreeNode>>();
        enum STATUS run() override;

        void addChild(std::shared_ptr<BehaviorTreeNode> child);
};

class RandomSelector : public BehaviorTreeNode {
    private:
        std::deque<std::shared_ptr<BehaviorTreeNode>> children = std::deque<std::shared_ptr<BehaviorTreeNode>>();

        std::vector<int> indices = std::vector<int>();

        int current = -1;

        int numProcessed = 0;
    public:
        enum STATUS run() override;

        void addChild(std::shared_ptr<BehaviorTreeNode> child);
};

class Sequence : public BehaviorTreeNode {
    private:

        int current = 0;
    public:
    std::deque<std::shared_ptr<BehaviorTreeNode>> children = std::deque<std::shared_ptr<BehaviorTreeNode>>();
        enum STATUS run() override;

        void addChild(std::shared_ptr<BehaviorTreeNode> child);
};

class ActionTask : public BehaviorTreeNode {
    private:
        std::string action;
    public:
        enum STATUS run() override;

        ActionTask(std::string action);
};

class Condition : public BehaviorTreeNode {
    private:

    public:
        std::shared_ptr<GameValue> equivalence;
        std::shared_ptr<GameValue> upperBound;
        std::shared_ptr<GameValue> lowerBound;

        std::shared_ptr<GameValue> value;

        Condition();

        enum STATUS run() override;
};

class BehaviorTree {
    private:
        std::shared_ptr<BehaviorTreeNode> root;
    public:

        BehaviorTree(std::shared_ptr<BehaviorTreeNode> root);

        void runTree();
};