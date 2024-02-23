#pragma once
#include "SFML/Graphics.hpp"
#include "SFML/OpenGL.hpp"
#include <cmath>

struct SteeringData {
    sf::Vector2f linear = sf::Vector2f(0, 0);
    float angular = 0;
};

struct Kinematic {
    int id;

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

class Crumb : sf::CircleShape
{

    private:
        int id;
    
    public:
        Crumb(int id);

        //tell breadcrumb to render self, using current render window
        void draw(sf::RenderWindow* window);

        //set position of breadcrumb
        void drop(float x, float y);

        //set position of breadcrumb
        void drop(sf::Vector2f position);
};

class Boid
{
    public:

        Kinematic kinematic;
        SteeringData *steering;



        Boid(sf::RenderWindow* w, sf::Texture& tex, std::vector<Crumb>* crumbs);

        void update(float deltaTime);

        void draw();

        void move();

        void setPosition (sf::Vector2f newPos);

        sf::Vector2f getPosition();

        void setRotation(float rotation);

    private:
        static int numBoids;

        //indice variables
        int crumb_idx;
        
        //float variables
        float drop_timer;
        
        //renderable objects
        sf::Sprite sprite;
        sf::RenderWindow* window;    

        //point of breadcrumbs
        std::vector<Crumb>* breadcrumbs;
};