#pragma once

#include "Boid.h"

#include "SFML/Graphics.hpp"
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

    virtual void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) = 0;
};

//Class for matching position (X, Y coordinates).
class PositionMatch : public SteeringBehavior {
    public:
    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching orientation (Direction we are facing).
class OrientationMatch : public SteeringBehavior {
    public:
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

    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching velocity (position over time).
class VelocityMatch : public SteeringBehavior {
    public:
    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};
//Class for matching rotation (rotational velocity).
class RotationMatch : public SteeringBehavior {
    public:

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

    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Wander : public SteeringBehavior {
    public:
    static float randBinomial();

    void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};

class Separation : public SteeringBehavior {
    private:
        //How close the other boids need to be to be affected.
        static constexpr float THRESHOLD = 50.f;

        static constexpr float DECAY_COEFFICIENT = 10.f;
    public:


        void calculateAcceleration(SteeringData *steering, Kinematic character, Kinematic goal) override;
};