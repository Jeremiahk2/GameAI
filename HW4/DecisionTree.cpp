#include "DecisionTree.h"

//COMMON:
std::deque<std::string> DecisionTreeNode::actionQueue;

//ACTION NODES:

Action::Action(std::string action) {
    this->action = action;
}

std::shared_ptr<DecisionTreeNode> Action::makeDecision() {
    actionQueue.push_front(action);
    return std::shared_ptr<DecisionTreeNode>(NULL);
}

//DECISION NODES:
Decision::Decision() {
    //Set up cases to no value.
    equivalence.reset(new GameValue);
    equivalence->type = GameValue::NONE;
    upperBound.reset(new GameValue);
    upperBound->type = GameValue::NONE;
    lowerBound.reset(new GameValue);
    lowerBound->type = GameValue::NONE;
    value.reset(new GameValue);
    value->type = GameValue::NONE;
}

std::shared_ptr<DecisionTreeNode> Decision::getBranch() {
    if (value->type != GameValue::NONE) {
        //If this is an equivalence case
        if (equivalence->type != GameValue::NONE) {
            return GameValue::compare(*value, *equivalence) == 0 ? trueNode : falseNode;
        }
        //If this is is a bounded case.
        if (upperBound->type != GameValue::NONE && lowerBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *upperBound) == -1 && GameValue::compare(*value, *lowerBound) == 1 ? trueNode : falseNode;
        }
        //If this is an upper bounded case.
        if (upperBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *upperBound) == -1 ? trueNode : falseNode;
        }
        //If this is a lower bounded case.
        if (lowerBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *lowerBound) == 1 ? trueNode : falseNode;
        }
        else {
            throw std::invalid_argument("No case selected");
        }
    }
    else {
        throw std::invalid_argument("Value type was NONE");
    }
    return std::shared_ptr<DecisionTreeNode>(this);
}

std::shared_ptr<DecisionTreeNode> Decision::makeDecision() {
    std::shared_ptr<DecisionTreeNode> branch = getBranch();
    return branch->makeDecision();
}