#include "GameState.h"
#include <memory>

class DecisionTreeNode {
    private:

    public:
    virtual std::shared_ptr<DecisionTreeNode> makeDecision() = 0;
};

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