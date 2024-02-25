#include "Timeline.h"
#include "SteeringBehavior.h"

#include <cstdio>
#include <deque>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

//Notes: Center of mass = X1 + X2 + X3... / M1 + M2 + M3...
//In our case that means that the X coordinate for the center of mass is the sum of all the X coordinates divided by the number of sprites. The average.
//Same with the Y coordinate.
// "Neighborhood" of a boid can just be a simple radius. Reynolds suggests making it a cone but fuck him

int main() {

    srand(time(0));
    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    sf::RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(640, 480, desktop.bitsPerPixel), "Game Window", sf::Style::Default);
    //Set framerate to 100 (ideally)
    window.setFramerateLimit(100);

    //Set up boid texture
    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png"))
    {
        // error...
    }

    //Set up boid
    int numBoids = 40;
    for (int i = 0; i < numBoids; i++) {
        SteeringBehavior::boids.push_back(new Boid(&window, texture));
        sf::Vector2f vector = SteeringBehavior::boids.back()->kinematic.pos;
        vector.x += Wander::randBinomial();
        vector.y += Wander::randBinomial();
        SteeringBehavior::boids.back()->kinematic.pos = vector;
    }
    std::deque<sf::CircleShape> clickCircles;

    //Set up steering behaviors.
    Wander wander;
    Separation separation;
    Flocking flocking;
    Kinematic target;


    //Create a timeline with a tic size of 10, updating every 10 ms or 100 fps.
    Timeline global;
    int tic = 10;
    Timeline frameTime(&global, tic);

    //CurrentTic starts higher than lastTic so the program starts immediately.
    int64_t currentTic = 0;
    int64_t lastTic = -1;

    while (window.isOpen()) {
        //Get the current tic.
        currentTic = frameTime.getTime();
        //Only process if we've gone to the next tic (or further).
        if (currentTic > lastTic) {
            sf::Event event;
            //If the X button is pressed, close the window
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.mouseButton.button == sf::Mouse::Left) {
                    target.pos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                    //Update mouse click circles.
                    std::cout << "X: " << target.pos.x << " Y: " << target.pos.y << std::endl;
                }
            }

            //Calculate acceleration for all boids.
            for (Boid *b : SteeringBehavior::boids) {
                flocking.calculateAcceleration(b->steering, b->kinematic, target);
            }
            //Update all boids
            for (Boid *b : SteeringBehavior::boids) {
                b->update(frameTime.getRealTicLength() * (float)(currentTic - lastTic));
                if (b->kinematic.id == 0) {
                    std::cout << "X Velocity: " << b->kinematic.velocity.x << " Y Velocity: " << b->kinematic.velocity.y << std::endl;
                }
            }
            //Draw to window.
            window.clear(sf::Color(0, 128, 128));
            for (Boid *b : SteeringBehavior::boids) {
                b->draw();
            }
            window.display();
        }
        

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}