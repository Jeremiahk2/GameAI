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

    //Set up bread crumbs
    std::vector<Crumb> breadcrumbs = std::vector<Crumb>();
    for(int i = 0; i < 10; i++)
    {
        Crumb c(i);
        breadcrumbs.push_back(c);
    }
    //Set up boid
    Boid b = Boid(&window, texture, &breadcrumbs);

    std::deque<sf::CircleShape> clickCircles;

    //Set up steering behaviors.
    Wander wander;
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
            }

            //Wander Steering behavior.
            wander.calculateAcceleration(b.steering, b.kinematic, target);

            //Update character with new steering behavior.
            b.update(frameTime.getRealTicLength() * (float)(currentTic - lastTic));

            //Draw to window.
            window.clear(sf::Color(0, 128, 128));
            b.move();
            for(int i = 0; i < breadcrumbs.size(); i++)
            {
                breadcrumbs[i].draw(&window);
            }
            b.draw();
            window.display();
        }
        

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}