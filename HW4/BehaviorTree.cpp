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
        enum STATUS status = children[indices[current]]->run();
        //If waiting, just return.
        if (status == STATUS::WAITING) {
            return status;
        }
        //If successful, the selector is successful. Reset index list, current, and return.
        else if (status == STATUS::SUCCESS) {
            indices.clear();
            for (int i = 0; i < children.size(); i++) {
                indices.push_back(i);
            }
            current = -1;
            return status;
        }
        //If failure, the selector may still succeed. Erase the failed index and keep going like normal.
        else if (status == STATUS::FAILURE){
            indices.erase(indices.begin() + current);
            current = -1;
        }
    }
    //If this is a fresh run.
    for (int i = children.size() - indices.size(); i < children.size(); i++) {
        int index = rand() % indices.size();
        enum STATUS status = children[indices[rand() % indices.size()]]->run();
        if (status == STATUS::WAITING) {
            current = index;
            return status;
        }
        if (status == STATUS::SUCCESS) {
            indices.clear();
            for (int i = 0; i < children.size(); i++) {
                indices.push_back(i);
            }
            current = -1;
            return STATUS::SUCCESS;
        }
        if (status == STATUS::FAILURE) {
            indices.erase(indices.begin() + index);
        }
    }
    indices.clear();
    for (int i = 0; i < children.size(); i++) {
        indices.push_back(i);
    }
    current = -1;
    return STATUS::FAILURE;
}

void RandomSelector::addChild(std::shared_ptr<BehaviorTreeNode> child) {
    indices.push_back((int)children.size());
    children.push_back(child);
}

enum STATUS Sequence::run() {
    for (int i = current; i < children.size(); i++) {
        enum STATUS status = children[i]->run();
        if (status == STATUS::WAITING) {
            current = i;
            return status;
        }
        if (status  == STATUS::FAILURE) {
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

