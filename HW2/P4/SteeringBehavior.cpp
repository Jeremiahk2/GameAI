#include "SteeringBehavior.h"

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
            float strength = fmin(DECAY_COEFFICIENT / (distance * distance), MAX_ACCEL);
            steering->linear += strength * normalize(direction);
        }
    }
}

void Flocking::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {

    //Begin weighted blending.

    PositionMatch cohesion;
    VelocityMatch aligner;
    Separation separator;
    OrientationMatch orienter;

    //Calculate center of mass of nearby neighbors and average velocity.
    sf::Vector2f averageVelocity = sf::Vector2f(0.f, 0.f);
    sf::Vector2f centerOfMass = sf::Vector2f(0.f, 0.f);
    int numNeighbors = 0;
    for (Boid *b : boids) {
        sf::Vector2f direction = b->kinematic.pos - character.pos;
        float distance = findMagnitude(direction);
        // std::cout << distance << std::endl;

        //NumNeighbors should always end up at one because we are included in the list of boids.
        if (distance < THRESHOLD) {
            centerOfMass += b->kinematic.pos;
            averageVelocity += b->kinematic.velocity;
            numNeighbors++;
            // std::cout << "Center of mass: " << b->kinematic.pos.x << " Average Velocity: " << b->kinematic.pos.y << std::endl;
        }
    }
    // std::cout << "NumNeighbors: " << numNeighbors << std::endl;
    // std::cout << "Center of mass: " << centerOfMass.x << " Average Velocity: " << averageVelocity.x << std::endl;
    centerOfMass /= (float)numNeighbors;
    averageVelocity /= (float)numNeighbors;
    // std::cout << "Center of mass: " << centerOfMass.x << " Average Velocity: " << averageVelocity.x << std::endl;

    goal.pos = centerOfMass;
    goal.velocity = averageVelocity;

    //Do weighted blending for linear acceleration behaviors.
    separator.calculateAcceleration(steering, character, goal); //Separation
    SteeringData separationData = *steering;
    aligner.calculateAcceleration(steering, character, goal); //Velocity matching C.O.M
    SteeringData alignmentData = *steering;
    cohesion.calculateAcceleration(steering, character, goal); //Position matching C.O.M
    SteeringData cohesionData = *steering;


    //Find total linear value.
    steering->linear = SEPARATION_WEIGHT * separationData.linear 
        + ALLIGNMENT_WEIGHT * alignmentData.linear + COHESION_WEIGHT * separationData.linear;

    float theta;
    if ((character.velocity).x != 0) {
        theta = atan2((character.velocity).y, (character.velocity).x);
    }
    else {
        theta = atan2((character.velocity).y, 0);
    }
    goal.orientation = theta;

    //Find total angular value.
    orienter.calculateAcceleration(steering, character, goal);
    
    //Questions:
    //Should I count in the center of mass/average velocity calculation? 
    //Should I allign in the direction of steering->linear? 
    //Should I wander?

    //Answers:
    //Doesn't matter much, but probably yes. He did mention some situation where you might not though.
    //Allign in direction of CURRENT velocity, not acceleration
    //No need to wander.
}














