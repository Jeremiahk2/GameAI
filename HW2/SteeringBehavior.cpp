#include "SteeringBehavior.h"

#define RDEC 100 //Free parameter. The radius around the target where we start to decelerate
#define RSAT 100 //Free parameter. The "good enough" range around the target.
#define TTTV 3 //Time To Target Velocity. Over what period of time do we want the change in velocity to occur.
#define MAXVEL 60 //Free parameter?
#define MAXACCEL 1 //Free parameter?
#define MAXROT 1 //Free parameter

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
    float magnitude = findMagnitude(direction);
    if (magnitude != 0) {
        goalVelocity /= magnitude;
    }
    goalVelocity *= goalSpeed;

    steering.linear = goalVelocity - character.velocity;
    steering.linear /= (float)TTTV;
    return steering;
}
//Actually just Align
SteeringData OrientationMatch::calculateAcceleration(Kinematic character, Kinematic goal) {
    SteeringData steering;
    float goalRotation;
    float rotation = goal.orientation - character.orientation;
    rotation = mapToRange(rotation);
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