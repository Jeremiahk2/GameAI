#pragma once

#include "Boid.h"

#include "SFML/Graphics.hpp"
#include "Pathfinding.h"
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

class SteeringBehavior {
    public:
        static std::vector<Boid *> boids;

        static float findMagnitude(sf::Vector2f vector) {
            float rtn =  sqrt( pow( vector.x, 2 ) + pow( vector.y, 2 ) );
            return rtn;
        }

        static float mapToRange(float theta) {
            theta = fmod(theta, 2 * M_PI);
            if (fabs(theta) <= M_PI) { //If theta is from -pi to pi
                return theta;
            }
            else if (theta > M_PI) { //if theta is greater than pi
                return theta - 2 * M_PI;
            }
            return theta + 2 * M_PI; //if theta is less than -pi
        }

        static sf::Vector2f normalize(sf::Vector2f vector) {
            return vector / findMagnitude(vector);
        }

    virtual void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) = 0;
};

//Class for matching position (X, Y coordinates).
class PositionMatch : public SteeringBehavior {
    private:
        static constexpr float RSAT = 0.f;
        static constexpr float RDEC = 0.f;
        static constexpr float MAXVEL = 160.f;
        static constexpr float TTTV = 1.f;
    public:
        void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching orientation (Direction we are facing).
class OrientationMatch : public SteeringBehavior {
    private:
        static constexpr float ANGULARDEC = 1.f;
        static constexpr float MAXROT = .8f;
        static constexpr float ANGULARSAT = .2f;
        static constexpr float TTTV = 1.f;
    public:

        void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching velocity (position over time).
class VelocityMatch : public SteeringBehavior {
    private:
        static constexpr float TTTV = 3.f;
    public:
    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching rotation (rotational velocity).
class RotationMatch : public SteeringBehavior {
    private:
        static constexpr float TTTV = 3.f;
    public:

    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Wander : public SteeringBehavior {
    private:
        static constexpr float WANDER_RATE = 1.25f;
        static constexpr float WANDER_OFFSET = 100.f;
        static constexpr float WANDER_RADIUS = 50.f;
        float wanderOrientation = 0;
    public:
    static float randBinomial();

    Wander();

    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Separation : public SteeringBehavior {
    private:
        //How close the other boids need to be to be affected.
        static constexpr float THRESHOLD = 25.f;

        static constexpr float DECAY_COEFFICIENT = 2000.f;

        static constexpr float MAX_ACCEL = 100.f;
    public:


        void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Flocking : public SteeringBehavior {
    private:
        //Weight for separation behavior.
        static constexpr float SEPARATION_WEIGHT = 4.0f;
        //Weight for velocity matching behavior.
        static constexpr float ALLIGNMENT_WEIGHT = 3.5f;
        //Weight for position matching behavior.
        static constexpr float COHESION_WEIGHT = .4f;
        //Threshold for position matching and allignment behavior.
        static constexpr float THRESHOLD = 30.f;

        static constexpr float ANG_THRESHOLD = (3 * M_PI) / 4;

    public:
        void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Pathfollowing : public SteeringBehavior {
    private:

    public:
        void calculateAcceleration(SteeringData *sterring, Kinematic character, Kinematic goal) override;

        static float euclidean(sf::Vector2f source, sf::Vector2f goal);

        int followPath(std::deque<Edge::Vertex> path, int pathOffset, float predictTime, Kinematic character);
};