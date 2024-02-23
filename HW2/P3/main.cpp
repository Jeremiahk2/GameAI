#include "Timeline.h"
#include "SteeringBehavior.h"

#include <cstdio>
#include <deque>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

//Breadcrumb class
class crumb : sf::CircleShape
{
    public:
        crumb(int id)
        {
            //set initial position and size breadcrumbs   
            this->id = id;         
            this->setRadius(10.f);
            this->setOrigin(10.f, 10.f);
            this->setFillColor(sf::Color::Red);
            this->setPosition(-100, -100);
        }

        //tell breadcrumb to render self, using current render window
        void draw(sf::RenderWindow* window)
        {
            window->draw(*this);
        }

        //set position of breadcrumb
        void drop(float x, float y)
        {
            this->setPosition(x, y);
        }

        //set position of breadcrumb
        void drop(sf::Vector2f position)
        {
            this->setPosition(position);
        }

    private:
        int id;
};

class boid
{
    public:
        boid(sf::RenderWindow* w, sf::Texture& tex, std::vector<crumb>* crumbs)
        {
            window = w;
            drop_timer = 15.f;
            crumb_idx = 0;
            sprite.setScale(0.05f, 0.05f);
            sprite.setOrigin(sf::Vector2f(17.0/ .05f, 17.0 / .05f));
            sprite.setPosition(sf::Vector2f(200.f, 200.f));          
            sprite.setTexture(tex);
            sprite.setScale(0.05f, 0.05f);
            breadcrumbs = crumbs;
        }

        void draw()
        {            
            window->draw(sprite);
        }  

        void move()
        {
            //basic timer for leaving breadcrumbs
            if (drop_timer > 0)
            {
                drop_timer -= 0.1f;
            }
            else
            {
                drop_timer = 15.f;
                breadcrumbs->at(crumb_idx).drop(sprite.getPosition());

                if (crumb_idx < 9)
                    crumb_idx++;
                else
                    crumb_idx = 0;
            }
        }

        void setPosition (sf::Vector2f newPos) {
            sprite.setPosition(newPos);
        }

        sf::Vector2f getPosition() {
            return sprite.getPosition();
        }

        void setRotation(float rotation) {
            sprite.setRotation(rotation);
        }

    private:
        //indice variables
        int crumb_idx;
        
        //float variables
        float drop_timer;
        
        //renderable objects
        sf::Sprite sprite;
        sf::RenderWindow* window;    

        //point of breadcrumbs
        std::vector<crumb>* breadcrumbs;
};

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
    std::vector<crumb> breadcrumbs = std::vector<crumb>();
    for(int i = 0; i < 10; i++)
    {
        crumb c(i);
        breadcrumbs.push_back(c);
    }
    //Set up boid
    boid b = boid(&window, texture, &breadcrumbs);

    std::deque<sf::CircleShape> clickCircles;

    Kinematic character;
    character.pos = b.getPosition();

    SteeringData steering;

    Wander wander;

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
            }

            //Update steering
            wander.calculateAcceleration(&steering, character, target);

            //Update character position and orientation.
            character.update(steering, frameTime.getRealTicLength() * (float)(currentTic - lastTic));
            b.setRotation(character.orientation * (180.0 / M_PI));
            //Make the X and Y value go from X = -640 to 640, and Y go from -480.f to 480.f
            b.setPosition(sf::Vector2f(fmod(character.pos.x, 640.f), fmod(character.pos.y, 480.f)));
            //Add max value to both X and Y to get rid of negatives, then mod again in case it was positive.
            b.setPosition(sf::Vector2f(fmod(b.getPosition().x + 640.f, 640.f), fmod(b.getPosition().y + 480.f, 480.f)));

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