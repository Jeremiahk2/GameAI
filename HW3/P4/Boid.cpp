#include "Boid.h"
#include <iostream>
Crumb::Crumb(int id)
{
    //set initial position and size breadcrumbs   
    this->id = id;         
    this->setRadius(2.f);
    this->setOrigin(this->getGlobalBounds().width / 2.f, this->getGlobalBounds().height / 2.f);
    this->setFillColor(sf::Color::White);
    this->setPosition(-100, -100);
}

//tell breadcrumb to render self, using current render window
void Crumb::draw(sf::RenderWindow* window)
{
    window->draw(*this);
}

//set position of breadcrumb
void Crumb::drop(float x, float y)
{
    this->setPosition(x, y);
}

//set position of breadcrumb
void Crumb::drop(sf::Vector2f position)
{
    this->setPosition(position);
}

int Boid::numBoids = 0;

Boid::Boid(sf::RenderWindow* w, sf::Texture& tex)
{
    window = w;
    drop_timer = 10.f;
    crumb_idx = 0;
    sprite.setTexture(tex);
    sprite.setScale(0.02f, 0.02f);
    sprite.setOrigin(sf::Vector2f((sprite.getGlobalBounds().width / 2.f) / .02f, (sprite.getGlobalBounds().height / 2.f) / .02f));
    // std::cout << sprite.getGlobalBounds().width << std::endl;
    kinematic.pos = sf::Vector2f(200.f, 200.f);
    steering = new SteeringData;
    kinematic.id = numBoids++;

    for(int i = 0; i < 125; i++)
    {
        Crumb c(i);
        breadcrumbs.push_back(c);
    }
}

void Boid::update(float deltaTime) {
    kinematic.update(*steering, deltaTime);

    

    //Set rotation, then boundary corrections.
    sprite.setRotation(kinematic.orientation * (180.0 / M_PI));
    //Make the X and Y value go from X = -640 to 640, and Y go from -480.f to 480.f
    sprite.setPosition(sf::Vector2f(fmod(kinematic.pos.x, window->getSize().x), fmod(kinematic.pos.y, window->getSize().y)));
    //Add max value to both X and Y to get rid of negatives, then mod again in case it was positive.
    sprite.setPosition(sf::Vector2f(fmod(sprite.getPosition().x + window->getSize().x, window->getSize().x), fmod(sprite.getPosition().y + window->getSize().y, window->getSize().y)));

    kinematic.pos = sprite.getPosition();
}

void Boid::draw()
{      
    //basic timer for leaving breadcrumbs
    if (drop_timer > 0)
    {
        drop_timer -= 0.1f;
    }
    else
    {
        drop_timer = 10.f;
        breadcrumbs.at(crumb_idx).drop(sprite.getPosition());

        if (crumb_idx < 124)
            crumb_idx++;
        else
            crumb_idx = 0;
    }

    for(int i = 0; i < breadcrumbs.size(); i++)
    {
        breadcrumbs.at(i).draw(window);
    }
    window->draw(sprite);
}  

void Boid::setPosition (sf::Vector2f newPos) {
    sprite.setPosition(newPos);
}

sf::Vector2f Boid::getPosition() {
    return sprite.getPosition();
}

void Boid::setRotation(float rotation) {
    sprite.setRotation(rotation);
}
