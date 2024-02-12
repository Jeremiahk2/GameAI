#pragma once
#include "SFML/Graphics.hpp"
#include <SFML/OpenGL.hpp>


class SteeringBehavior {
    public:

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
    virtual SteeringData calculateAcceleration(Kinematic character, Kinematic goal) = 0;
};

class PositionMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};

class OrientationMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};

class VelocityMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};

class RotationMatch : public SteeringBehavior {
    SteeringData calculateAcceleration(Kinematic character, Kinematic goal) override;
};