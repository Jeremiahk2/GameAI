#include <cstdio>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

int main() {
    // Create a window with the same pixel depth as the desktop, with 144 frames per second.
    sf::RenderWindow window;
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.create(sf::VideoMode(640, 480, desktop.bitsPerPixel), "Game Window", sf::Style::Default);
    //Set framerate to 100 (ideally)
    window.setFramerateLimit(100);


    //Set up sprite
    sf::Sprite rectangle;
    sf::Texture texture;
    if (!texture.loadFromFile("Assets/boid.png")) {
        printf("Failed");
        return -1;
    }
    rectangle.setTexture(texture);
    rectangle.setScale(.05, .05);
    //Set up positions
    sf::Vector2f initPos(20.f, 20.f);
    rectangle.setPosition(initPos);

    //Begin main game loop
    int count = 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear(sf::Color(0, 128, 128));
        rectangle.move(sf::Vector2f(1.f, 0.f));
        window.draw(rectangle);
        window.display();
        count++;
        if (count > 620 - rectangle.getGlobalBounds().width) {
            count = 0;
            rectangle.setPosition(initPos);
        }
    }
    return EXIT_SUCCESS;
}