#include "Timeline.h"
#include "SteeringBehavior.h"
#include "Pathfinding.h"

#include <cstdio>
#include <deque>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

//Notes: Center of mass = X1 + X2 + X3... / M1 + M2 + M3...
//In our case that means that the X coordinate for the center of mass is the sum of all the X coordinates divided by the number of sprites. The average.
//Same with the Y coordinate.
// "Neighborhood" of a boid can just be a simple radius. Reynolds suggests making it a cone but we not doing that.

int main() {

    srand(time(0));
    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    sf::RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    int winWidth = 640;
    int winHeight = 480;
    window.create(sf::VideoMode(640, 480, desktop.bitsPerPixel), "Game Window", sf::Style::Default);

    //Set up boid texture
    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png"))
    {
        std::cout << "Error" << std::endl;
    }
    //Set up boid
    Boid b(&window, texture);
    SteeringBehavior::boids.push_back(&b);
    //Set up clickCircles 
    std::deque<sf::CircleShape> clickCircles;

    //Set up steering behaviors.
    Kinematic target;
    target.pos = sf::Vector2f(0.f, 0.f);

    //Set up environment
    std::deque<sf::RectangleShape> tiles;
    float tileSize = b.sprite.getGlobalBounds().width;
    std::cout << tileSize << std::endl;
    int horizontalTiles = winWidth / tileSize + 1;
    int verticalTiles = winHeight / tileSize + 1;

    for (int i = 0; i < horizontalTiles; i++) {
        for (int j = 0; j < verticalTiles; j++) {
            sf::RectangleShape tile;
            tile.setSize(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(i * tileSize, j * tileSize);
            if (j == 0 || j == verticalTiles - 1 || i == 0 || i == horizontalTiles - 1) {
                tile.setFillColor(sf::Color::Red);
            }
            else {
                tile.setFillColor(sf::Color(0, 128, 128));
            }
            tile.setOutlineThickness(1.f);
            tile.setOutlineColor(sf::Color::Black);
            tiles.push_back(tile);
        }
    }

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
                
            }
            //Update all boids
            for (Boid *b : SteeringBehavior::boids) {
                b->update(frameTime.getRealTicLength() * (float)(currentTic - lastTic));
            }
            //Draw to window.
            window.clear(sf::Color(0, 128, 128));
            for (int i = 0; i < tiles.size(); i++) {
                window.draw(tiles[i]);
            }
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