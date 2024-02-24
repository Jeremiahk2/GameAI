#include "SteeringBehavior.h"

std::vector<Boid *> SteeringBehavior::boids = std::vector<Boid *>();

void PositionMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    // sf::Vector2f direction = goal.pos - character.pos;
    sf::Vector2f direction;
    //Get the direction for X directly and by wrapping around
    float directX = goal.pos.x - character.pos.x; 
    if (directX == 0.f) {
        direction.x = 0.f;
    }
    else {
        float indirectX = directX - (directX/ fabs(directX)) * 640.f;
        direction.x = (fabs(directX) < fabs(indirectX)) ? directX : indirectX;
    }
    //Get the direction for Y directly and by wrapping around.
    float directY = goal.pos.y - character.pos.y; 
    if (directY == 0.f) {
        direction.y = 0.f;
    }
    else {
        float indirectY = directY - (directY/ fabs(directY)) * 480.f;
        //Use the smaller (absolute) one.
        direction.y = (fabs(directY) < fabs(indirectY)) ? directY : indirectY;
    }

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
            continue;
        }
        sf::Vector2f direction;
        //Get the direction for X directly and by wrapping around
        float directX = character.pos.x - b->kinematic.pos.x; 
        if (directX == 0.f) {
            direction.x = 0.f;
        }
        else {
            float indirectX = directX - (directX/ fabs(directX)) * 640.f;
            direction.x = (fabs(directX) < fabs(indirectX)) ? directX : indirectX;
        }
        //Get the direction for Y directly and by wrapping around.
        float directY = character.pos.y - b->kinematic.pos.y; 
        if (directY == 0.f) {
            direction.y = 0.f;
        }
        else {
            float indirectY = directY - (directY/ fabs(directY)) * 480.f;
            //Use the smaller (absolute) one.
            direction.y = (fabs(directY) < fabs(indirectY)) ? directY : indirectY;
        }

        float distance = findMagnitude(direction);

        if (distance < THRESHOLD && distance != 0) {
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
        bool invertedX = false;
        bool invertedY = false;
        sf::Vector2f direction;
        //Get the direction for X directly and by wrapping around
        float directX = b->kinematic.pos.x - character.pos.x; 
        if (directX == 0.f) {
            direction.x = 0.f;
        }
        else {
            invertedX = true;
            float indirectX = directX - (directX/ fabs(directX)) * 640.f;
            direction.x = (fabs(directX) < fabs(indirectX)) ? directX : indirectX;
        }
        //Get the direction for Y directly and by wrapping around.
        float directY = b->kinematic.pos.y - character.pos.y; 
        if (directY == 0.f) {
            direction.y = 0.f;
        }
        else {
            invertedY = true;
            float indirectY = directY - (directY/ fabs(directY)) * 480.f;
            //Use the smaller (absolute) one.
            direction.y = (fabs(directY) < fabs(indirectY)) ? directY : indirectY;
        }

        float distance = findMagnitude(direction);
        //NumNeighbors should always end up at one because we are included in the list of boids.
        if (distance < THRESHOLD) {
            centerOfMass += character.pos + direction;
            averageVelocity += b->kinematic.velocity;
            numNeighbors++;
        }
    }
    centerOfMass /= (float)numNeighbors;
    averageVelocity /= (float)numNeighbors;

    goal.pos = centerOfMass;
    goal.velocity = averageVelocity;

    //Do weighted blending for linear acceleration behaviors.
    SteeringData separationData;
    separator.calculateAcceleration(&separationData, character, goal); //Separation

    SteeringData alignmentData;
    aligner.calculateAcceleration(&alignmentData, character, goal); //Velocity matching C.O.M

    SteeringData cohesionData;
    if (numNeighbors != 1) {
        cohesion.calculateAcceleration(&cohesionData, character, goal); //Position matching C.O.M
    }

    separationData.linear *= SEPARATION_WEIGHT;
    alignmentData.linear *= ALLIGNMENT_WEIGHT;
    cohesionData.linear *= COHESION_WEIGHT;
    // std::cout << "Boid " << character.id << " Position: " << character.pos.x << std::endl;
    // std::cout << "Boid " << character.id << " Separation: " << separationData.linear.x << std::endl;
    // std::cout << "Boid " << character.id << " Alignment: " << alignmentData.linear.x << std::endl;
    // std::cout << "Boid " << character.id << " Cohesion: " << cohesionData.linear.x << std::endl;
    
    //Find total linear value.
    steering->linear = SEPARATION_WEIGHT * separationData.linear
        + ALLIGNMENT_WEIGHT * alignmentData.linear + COHESION_WEIGHT * cohesionData.linear;

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














