#include "BehaviorTree.h"

std::map<std::string, enum STATUS> BehaviorTreeNode::actionQueue;

enum STATUS Selector::run() {
    for (int i = current; i < children.size(); i++) {
        enum STATUS status = children[i]->run();
        if (status == STATUS::WAITING) {
            current = i;
            return status;
        }
        if (status == STATUS::SUCCESS) {
            current = 0;
            return STATUS::SUCCESS;
        }
    }
    current = 0;
    return STATUS::FAILURE;
}

void Selector::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    children.push_back(child);
}

enum STATUS RandomSelector::run() {
    //If we are returning here after an action made us wait.
    if (current >= 0) {
        enum STATUS status = children[current]->run();
        if (status != STATUS::WAITING) {
            current = -1;
        }
        return status;
    }
    //If this is a fresh run.
    std::vector<int> indices;
    for (int i = 0; i < children.size(); i++) {
        indices.push_back(i);
    }
    for (int i = 0; i < children.size(); i++) {
        int index = rand() % indices.size();
        enum STATUS status = children[indices[rand() % indices.size()]]->run();
        indices.erase(indices.begin() + index);
        if (status == STATUS::WAITING) {
            current = index;
            return status;
        }
        if (status == STATUS::SUCCESS) {
            return STATUS::SUCCESS;
        }
    }
    return STATUS::FAILURE;
}

void RandomSelector::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    children.push_back(child);
}

enum STATUS Sequence::run() {
    for (int i = current; i < children.size(); i++) {
        enum STATUS status = children[i]->run();
        if (status == STATUS::WAITING) {
            current = i;
            return status;
        }
        if (status != STATUS::SUCCESS) {
            current = 0;
            return STATUS::FAILURE;
        }
    }
    current = 0;
    return STATUS::SUCCESS;
}

void Sequence::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    children.push_back(child);
}

ActionTask::ActionTask(std::string action) {
    this->action = action;
}

enum STATUS ActionTask::run() {
    //No action has been queued, so this is the first run through. Queue the new action and wait.
    if (actionQueue.find(action) == actionQueue.end()) {
        actionQueue.insert({action, STATUS::WAITING});
        return STATUS::WAITING;
    }
    //The action has been completed, but failed.
    if (actionQueue[action] == STATUS::FAILURE) {
        actionQueue.erase(action);
        return STATUS::FAILURE;
    }
    //The action has been completed, and was successful.
    else if (actionQueue[action] == STATUS::SUCCESS) {
        actionQueue.erase(action);
        return STATUS::SUCCESS;
    }
    //If it's waiting to finish, just return that.
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

