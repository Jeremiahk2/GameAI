#include "SteeringBehavior.h"

SteeringData PositionMatch::calculateAcceleration(Kinematic character, Kinematic goal) {
    SteeringData steering;
    sf::Vector2f direction = goal.pos - character.pos;
    float distance = findMagnitude(direction);
    float goalSpeed;
    if (distance < RSAT) {
        goalSpeed = 0;
    }
    else if (distance > RDEC) {
        goalSpeed = MAXVEL;
    }
    else {
        goalSpeed = MAXVEL * (distance / RDEC);
    }

    sf::Vector2f goalVelocity = direction;
    goalVelocity /= findMagnitude(direction);
    goalVelocity *= goalSpeed;

    steering.linear = goalVelocity - character.velocity;
    steering.linear /= (float)TTTV;
    return steering;
}
//Actually just Align
SteeringData OrientationMatch::calculateAcceleration(Kinematic character, Kinematic goal) {
    SteeringData steering;
    float goalRotation;
    float rotation = goal.orient - char.orient;
    rotation = mapToRange(orient);
    float rotationSize = abs(rotation);
    if (rotationSize < RSAT) {
        goalRotation = 0;
    }
    else if (rotationSize > RDEC) {
        goalRotation = MAXROT;
    }
    else {
        goalRotation = MAXROT * rotationSize/RDEC;
    }

    goalRotation *= rotation/abs(rotation);
    steering.angular = goalRotation - character.rotation;
    steering.angular /= (float)TTTV;
    return steering;
}

SteeringData VelocityMatch::calculateAcceleration(Kinematic character, Kinematic goal) {
    SteeringData steering;
    steering.linear = goal.velocity - character.velocity;
    steering.linear /= (float)TTTV;
    return steering;
}

SteeringData RotationMatch::calculateAcceleration(Kinematic character, Kinematic goal) {
    SteeringData data;
    return data;
}