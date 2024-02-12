#pragma once
#include "SFML/Graphics.hpp"
#include <SFML/OpenGL.hpp>
#include <cmath>

#define RDEC 1 //Free parameter. The radius around the target where we start to decelerate
#define RSAT 1 //Free parameter. The "good enough" range around the target.
#define TTTV 1 //Time To Target Velocity. Over what period of time do we want the change in velocity to occur.
#define MAXVEL 1 //Free parameter?
#define MAXACCEL 1 //Free parameter?
#define MAXROT 1 //Free parameter

struct SteeringData {
    sf::Vector2f linear;
    float angular;
};

struct Kinematic {
    sf::Vector2f pos;
    float orientation;
    sf::Vector2f velocity;
    float rotation;
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
        return ( sqrt( pow( vector.x, 2 ) + pow( vector.y, 2 ) ));
    }

    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic goal) = 0;
};

//Class for matching position (X, Y coordinates).
class PositionMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};
//Class for matching orientation (Direction we are facing).
class OrientationMatch : public SteeringBehavior {
    static float mapToRange(float theta) {
        theta = fmod(theta, 2 * M_PI);
        if (abs(theta) <= M_PI) { //If theta is from -pi to pi
            return theta;
        }
        else if (theta > M_PI) { //if theta is greater than pi
            return theta - 2 * M_PI;
        }
        return theta + 2 * M_PI; //if theta is less than -pi
    }

    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};
//Class for matching velocity (position over time).
class VelocityMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};
//Class for matching rotation (rotational velocity).
class RotationMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};