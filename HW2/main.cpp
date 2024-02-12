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

    //Sprite and Moves array are parallel to eachother.
    sf::Sprite sprites[4];
    sf::Vector2f moves[4];

    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png")) {
        return -1;
    }
    float xSpeed = 100.f;
    //Set up each sprite to start at the top left (offset), and set their default movement at 100.f in the x direction.
    for (int i = 0; i < 4; i++) {
        sprites[i].setTexture(texture);
        sprites[i].setScale(.05, .05);
        sprites[i].setPosition(sf::Vector2f(sprites[i].getGlobalBounds().width, sprites[i].getGlobalBounds().width));
        //Set speed to 100.f per second because we move every 10 ms, so 1.f per frame.
        moves[i].x = xSpeed;
    }
    //The width of the sprite is the biggest, so we use it as our offset.
    float offset = sprites[0].getGlobalBounds().width;

    //Counters
    int numSprites = 1;
    //"Start" is the starting point of our for loops. This goes up as sprites are removed from view.
    int start = 0;
    float xDistance = 0.f;

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
            window.clear(sf::Color(0, 128, 128));
            //Move all the sprites by their assigned movement vectors. Also draw them.
            for (int i = start; i < numSprites; i++) {
                sprites[i].move(moves[i] * (frameTime.getRealTicLength() * (float)(currentTic - lastTic))); //Move by speed per second multiplied by the time gone by.
                window.draw(sprites[i]);
            }
            window.display();
            xDistance += xSpeed * frameTime.getRealTicLength() * (float)(currentTic - lastTic);
            //If we've moved to the point where we reached the end of the screen, update the sprites.
            if (xDistance > 640 - offset * 2) {
                for (int i = start; i < numSprites; i++) {
                    //All sprites will rotate.
                    sprites[i].rotate(90);
                    //Sprites moving vertically need to now have their movement be negative.
                    if (moves[i].y != 0) {
                        moves[i].y *= -1;
                    }
                    //Swap x and y for all sprites
                    float temp = moves[i].x;
                    moves[i].x = moves[i].y;
                    moves[i].y = temp;
                    //Sprites moving in the Y direction need to swap to the Y movement speed and vice versa
                    if (moves[i].y != 0) {
                        moves[i].y *= ((480 - offset * 2) / (640 - offset * 2)) ;
                    }
                    else if (moves[i].x != 0) {
                        moves[i].x /= ((480 - offset * 2) / (640 - offset * 2));
                    }
                }
                //If we haven't reached 4 sprites, add another (4 is cap)
                if (numSprites < 4) {
                    numSprites++;
                }
                //If we have, remove the lowest sprite in the array from being rendered by increasing start.
                else {
                    start++;
                }
                //If all sprites have been removed, start again. All sprites should be at their original spot and facing right.
                if (numSprites == start) {
                    numSprites = 1;
                    start = 0;
                }
                xDistance = 0;
            }
        }
        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}