#include "SteeringBehavior.h"

#define RDEC 200 //Free parameter. The radius around the target where we start to decelerate
#define RSAT 10 //Free parameter. The "good enough" range around the target.
#define TTTV 1 //Time To Target Velocity. Over what period of time do we want the change in velocity to occur.
#define MAXVEL 60 //Free parameter?
#define MAXACCEL 1 //Free parameter?

#define ANGULARDEC 1
#define MAXROT .8 //Free parameter
#define ANGULARSAT .6

void PositionMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
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

    steering->linear = goalVelocity - character.velocity;
    steering->linear /= (float)TTTV;
}
//Actually just Align
void OrientationMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    float goalRotation;
    float rotation = goal.orientation - character.orientation;
    rotation = mapToRange(rotation);
    float rotationSize = fabs(rotation);
    if (rotationSize < ANGULARSAT) {
        goalRotation = 0;
    }
    else if (rotationSize > ANGULARDEC) {
        goalRotation = MAXROT;
    }
    else {
        goalRotation = MAXROT * rotationSize/ANGULARDEC;
    }

    if (rotation == 0) {
        goalRotation *= 0;
    }
    else {
        goalRotation *= rotation/fabs(rotation);
    }
    steering->angular = goalRotation - character.rotation;
    steering->angular /= (float)TTTV;
}

void VelocityMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    steering->linear = goal.velocity - character.velocity;
    steering->linear /= (float)TTTV;
}

void RotationMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    steering->angular = mapToRange(goal.rotation - character.rotation);
    steering->angular /= (float)TTTV;
}