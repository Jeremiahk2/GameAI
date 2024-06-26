#include "SteeringBehavior.h"

std::vector<Boid *> SteeringBehavior::boids = std::vector<Boid *>();

void PositionMatch::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    sf::Vector2f direction;

    if (character.pos - goal.pos == sf::Vector2f(0.f, 0.f)) {
        return;
    }
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

        if (distance <= THRESHOLD && distance != 0) {
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
    float theta;
    if ((character.velocity).x != 0) {
        theta = atan2((character.velocity).y, (character.velocity).x);
    }
    else {
        theta = atan2((character.velocity).y, 0);
    }
    goal.orientation = theta;


    //Calculate center of mass of nearby neighbors and average velocity.
    sf::Vector2f averageVelocity = sf::Vector2f(0.f, 0.f);
    sf::Vector2f centerOfMass = sf::Vector2f(0.f, 0.f);
    int numMass = 0;
    int numVelocity = 0;
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

        float angle;
        if (distance == 0.f) {
            angle = theta;
        }
        else if ((direction).x != 0.f) { 
            angle = atan2((direction).y, (direction).x);
        }
        else {
            angle = atan2((direction).y, 0.f);
        }

        if ((distance < THRESHOLD)) {
            averageVelocity += b->kinematic.velocity;
            numVelocity++;
            if ((mapToRange(angle) - mapToRange(theta)) < ANG_THRESHOLD) {
                centerOfMass += character.pos + direction;
                numMass++;
            }
        }
    }
    centerOfMass /= (float)numMass;
    averageVelocity /= (float)numVelocity;

    goal.pos = centerOfMass;
    goal.velocity = averageVelocity;

    //Do weighted blending for linear acceleration behaviors.
    SteeringData separationData;
    separator.calculateAcceleration(&separationData, character, goal); //Separation

    SteeringData alignmentData;
    if (numVelocity > 1) {
        aligner.calculateAcceleration(&alignmentData, character, goal); //Velocity matching C.O.M
    }

    SteeringData cohesionData;
    if (numMass > 1) {
        cohesion.calculateAcceleration(&cohesionData, character, goal); //Position matching C.O.M
    }

    separationData.linear *= SEPARATION_WEIGHT;
    alignmentData.linear *= ALLIGNMENT_WEIGHT;
    cohesionData.linear *= COHESION_WEIGHT;
    
    //Find total linear value.
    steering->linear = SEPARATION_WEIGHT * separationData.linear
        + ALLIGNMENT_WEIGHT * alignmentData.linear + COHESION_WEIGHT * cohesionData.linear;

    //Find total angular value.
    orienter.calculateAcceleration(steering, character, goal);
}

void Pathfollowing::calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) {
    Kinematic goalOrientation;
    float theta;
    if ((character.velocity).x != 0) {
        theta = atan2((character.velocity).y, (character.velocity).x);
    }
    else {
        theta = atan2((character.velocity).y, 0);
    }
    goalOrientation.orientation = theta;


    PositionMatch arrival;
    arrival.calculateAcceleration(steering, character, goal);

    OrientationMatch orienter;
    orienter.calculateAcceleration(steering, character, goalOrientation);
}

float Pathfollowing::euclidean(sf::Vector2f source, sf::Vector2f goal) {
    return sqrt(pow((source - goal).x, 2) + pow((source - goal).y, 2));
}

int Pathfollowing::followPath(std::deque<std::shared_ptr<Edge::Vertex>> path, int pathOffset, float predictTime, Kinematic character) {
    sf::Vector2f futurePos = character.pos + character.velocity * predictTime;
    int mindex = 0;
    float min = FLT_MAX;
    for (int i = 0; i < path.size(); i++) {
        //Pick the closest distance. If two are equal, pick the one ahead of you.
        float currentValue = Pathfollowing::euclidean(path[i]->position, futurePos);
        if (currentValue <= min) {
            min = currentValue;
            mindex = i;
        }
    }
    if (mindex + pathOffset > path.size() - 1) {
        return path.size() - 1;
    }
    return mindex + pathOffset;
}

