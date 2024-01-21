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

    sf::Sprite sprites[4];
    sf::Vector2f moves[4];
    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png")) {
        return -1;
    }

    for (int i = 0; i < 4; i++) {
        sprites[i].setTexture(texture);
        sprites[i].setScale(.05, .05);
        sprites[i].setPosition(sf::Vector2f(sprites[i].getGlobalBounds().width, sprites[i].getGlobalBounds().width));
        moves[i].x = 1.f;
    }

    float offset = sprites[0].getGlobalBounds().width;

    //Begin main game loop
    int numSprites = 1;
    int start = 0;
    int iterations = 0;

    while (window.isOpen()) {
        sf::Event event;
        //If the X button is pressed, close the window
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color(0, 128, 128));
        for (int i = start; i < numSprites; i++) {
            sprites[i].move(moves[i]);
            window.draw(sprites[i]);
        }
        window.display();
        iterations++;
        //If we've moved to the point where we reached the end of the screen, reset the iterations and pos.
        if (iterations > 640 - offset * 2) {
            for (int i = start; i < numSprites; i++) {
                sprites[i].rotate(90);
                if (moves[i].y != 0) {
                    moves[i].y *= -1;
                }
                float temp = moves[i].x;
                moves[i].x = moves[i].y;
                moves[i].y = temp;
                if (moves[i].y != 0) {
                    moves[i].y *= ((480 - offset * 2) / (640 - offset * 2)) ;
                }
                else if (moves[i].x != 0) {
                    moves[i].x /= ((480 - offset * 2) / (640 - offset * 2));
                }
            }
            if (numSprites < 4) {
                numSprites++;
            }
            else {
                start++;
            }
            if (numSprites == start) {
                numSprites = 1;
                start = 0;
            }
            iterations = 0;
        }
    }
    return EXIT_SUCCESS;
}