#include "BehaviorTree.h"

enum STATUS Selector::run() {
    for (int i = 0; i < children.size(); i++) {
        enum STATUS status = children[i]->run();
        if (status == STATUS::WAITING) {
            return status;
        }
        if (status == STATUS::SUCCESS) {
            return STATUS::SUCCESS;
        }
    }
    return STATUS::FAILURE;
}

void Selector::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    children.push_back(child);
}

enum STATUS Sequence::run() {
    for (int i = 0; i < children.size(); i++) {
        enum STATUS status = children[i]->run();
        if (status == STATUS::WAITING) {
            return status;
        }
        if (status != STATUS::SUCCESS) {
            return STATUS::FAILURE;
        }
    }
    return STATUS::SUCCESS;
}

void Sequence::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    children.push_back(child);
}

ActionTask::ActionTask(std::string action) {
    this->action = action;
}

enum STATUS ActionTask::run() {
    return STATUS::WAITING;
}

Condition::Condition() {
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



enum STATUS Condition::run() {
    if (value->type != GameValue::NONE) {
        //If this is an equivalence case
        if (equivalence->type != GameValue::NONE) {
            return GameValue::compare(*value, *equivalence) == 0 ? STATUS::SUCCESS : STATUS::FAILURE;
        }
        //If this is is a bounded case.
        if (upperBound->type != GameValue::NONE && lowerBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *upperBound) == -1 && GameValue::compare(*value, *lowerBound) == 1 ? STATUS::SUCCESS : STATUS::FAILURE;
        }
        //If this is an upper bounded case.
        if (upperBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *upperBound) == -1 ? STATUS::SUCCESS : STATUS::FAILURE;
        }
        //If this is a lower bounded case.
        if (lowerBound->type != GameValue::NONE) {
            return GameValue::compare(*value, *lowerBound) == 1 ? STATUS::SUCCESS : STATUS::FAILURE;
        }
        else {
            throw std::invalid_argument("No case selected");
        }
    }
    else {
        throw std::invalid_argument("Value type was NONE");
    }
    return STATUS::FAILURE;
}

BehaviorTree::BehaviorTree(std::shared_ptr<BehaviorTreeNode> root) {
    this->root = root;
}

void BehaviorTree::runTree() {
    root->run();
}

