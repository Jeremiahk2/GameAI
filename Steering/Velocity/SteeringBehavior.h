#pragma once
#include "SFML/Graphics.hpp"
#include <SFML/OpenGL.hpp>
#include <cmath>
#include <iostream>

struct SteeringData {
    sf::Vector2f linear = sf::Vector2f(0, 0);
    float angular = 0;
};

struct Kinematic {
    sf::Vector2f pos = sf::Vector2f(0, 0);
    float orientation = 0;
    sf::Vector2f velocity = sf::Vector2f(0, 0);
    float rotation = 0;
    void update( SteeringData steering,float deltaT) {
        //pos, orient
        pos += velocity * deltaT;
        orientation += rotation * deltaT;
        //vel, rot
        velocity += steering.linear * deltaT;
        rotation += steering.angular * deltaT;
    }
};

class SteeringBehavior {
    public:

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