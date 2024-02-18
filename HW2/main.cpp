#include "Timeline.h"
#include "SteeringBehavior.h"

#include <cstdio>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

int main() {
    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    sf::RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(640, 480, desktop.bitsPerPixel), "Game Window", sf::Style::Default);
    //Set framerate to 100 (ideally)
    window.setFramerateLimit(100);

    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png")) {
        return -1;
    }

    //Set up main sprite.
    sf::Sprite main;
    main.setTexture(texture);
    main.setScale(.05, .05);
    main.setOrigin(17 / .05, 17 / .05);
    main.setPosition(sf::Vector2f(main.getGlobalBounds().width, main.getGlobalBounds().width));

    Kinematic character;
    character.pos = main.getPosition();

    SteeringData steering;

    PositionMatch posMatcher;
    OrientationMatch orientMatcher;

    Kinematic target;
    target.pos = character.pos;
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
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        target.pos = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
                        // std::cout << "Target X: " << target.pos.x << " Target Y: " << target.pos.y << std::endl;
                    }
                }
            }
            //Get orientation angle.
            float theta;
            if ((target.pos - character.pos).x != 0) {
                theta = atan2((target.pos - character.pos).y, (target.pos - character.pos).x);
            }
            else {
                theta = atan2((target.pos - character.pos).y, 0);
            }
            target.orientation = theta;
            std::cout << "Theta: " << theta << std::endl;

            //Update steering
            posMatcher.calculateAcceleration(&steering, character, target);
            orientMatcher.calculateAcceleration(&steering, character, target);

            //Update character position and orientation.
            character.update(steering, frameTime.getRealTicLength() * (float)(currentTic - lastTic));
            main.setRotation(character.orientation * (180.0 / M_PI));
            main.setPosition(character.pos);

            //Draw to window.
            window.clear(sf::Color(0, 128, 128));
            window.draw(main);
            window.display();
        }
        

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}