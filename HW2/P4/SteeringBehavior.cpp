#include "SteeringBehavior.h"

#define MAXACCEL 100 //Free parameter?



std::vector<Boid *> SteeringBehavior::boids = std::vector<Boid *>();

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

Wander::Wander() {
}

float Wander::randBinomial() {
    float first = (float)(rand()) / (float)(RAND_MAX);
    float second = (float)(rand()) / (float)(RAND_MAX);
    return first - second;
}

void Wander::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    //Get how much we want to turn. If it's 1.25 and randBinomial returns 1, then we will turn 1.25 in the circle.
    steering->wanderOrientation += randBinomial() * WANDER_RATE;
    //Set our goal orientation at 1.25 from where we are currently facing.
    goal.orientation = steering->wanderOrientation + character.orientation;

    //Find center of circle
    goal.pos = character.pos + WANDER_OFFSET * sf::Vector2f(cos(character.orientation), sin(character.orientation));
    //Find target.
    goal.pos += WANDER_RADIUS * sf::Vector2f(cos(goal.orientation), sin(goal.orientation));

    OrientationMatch align;
    align.calculateAcceleration(steering, character, goal);

    PositionMatch arrive;
    arrive.calculateAcceleration(steering, character, goal);
}

void Separation::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    for (Boid *b : boids) {
        if (b->kinematic.id == character.id) {
            break;
        }
        sf::Vector2f direction = b->kinematic.pos - character.pos;
        float distance = findMagnitude(direction);

        if (distance < THRESHOLD) {
            float strength = fmin(DECAY_COEFFICIENT / (distance * distance), MAXACCEL);
            steering->linear += strength * normalize(direction);
        }
    }
}

