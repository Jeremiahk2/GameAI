// 
// @author Derek Martin (dmartin7@ncsu.edu)
//
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

const sf::Vector2f TOP_RIGHT = sf::Vector2f(550, 0);
const sf::Vector2f BOT_RIGHT = sf::Vector2f(550, 550);
const sf::Vector2f BOT_LEFT = sf::Vector2f(0, 550);
const sf::Vector2f TOP_LEFT = sf::Vector2f(0, 0);

//Breadcrumb class
class crumb : sf::CircleShape
{
    public:
        crumb(int id)
        {
            //set initial position and size breadcrumbs   
            this->id = id;         
            this->setRadius(10.f);
            this->setFillColor(sf::Color(0, 0, 255, 255));
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
            speed = 0.1f;
            drop_timer = 100.f;
            velocity = sf::Vector2f(1, 0);
            target_idx = 1;
            crumb_idx = 0;
            target = TOP_RIGHT;
            position = sf::Vector2f(0, 0);            
            sprite.setTexture(tex);
            sprite.setScale(0.05f, 0.1f);
            sprite.setOrigin()
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
                drop_timer = 100.f;
                breadcrumbs->at(crumb_idx).drop(position);

                if (crumb_idx < 9)
                    crumb_idx++;
                else
                    crumb_idx = 0;
            }

            //if reached target, switch to next target
            if (reached(position, target))
            {
                switch (target_idx)
                {
                    case 0:
                        target = TOP_RIGHT;
                        target_idx = 1;
                        velocity = sf::Vector2f(1, 0);
                        orientation = 0;                     
                        break;
                    case 1:
                        target = BOT_RIGHT;
                        target_idx = 2;
                        velocity = sf::Vector2f(0, 1);
                        orientation = 90;
                        break;
                    case 2:
                        target = BOT_LEFT;
                        target_idx = 3;
                        velocity = sf::Vector2f(-1, 0);
                        orientation = 180;
                        break;
                    case 3:
                        target = TOP_LEFT;
                        target_idx = 0;
                        velocity = sf::Vector2f(0, -1);
                        orientation = 270;
                        break;
                }

                sprite.setRotation(orientation);
            }
            //else move toward target
            else
            {
                position += velocity * speed;
                sprite.setPosition(position);
            }
        }

        //check if boid has reached target position
        bool reached(sf::Vector2f pos, sf::Vector2f tar)        
        {
            sf::Vector2f dis = tar - pos;
            if (abs(dis.x + dis.y) <= 1)
                return true;
            return false;
        }

    private:
        //indice variables
        int target_idx;
        int crumb_idx;
        
        //float variables
        float drop_timer;
        float speed;
        float orientation;
        
        //renderable objects
        sf::Sprite sprite;
        sf::RenderWindow* window;    
        
        //vector variables
        sf::Vector2f target;    
        sf::Vector2f position;
        sf::Vector2f velocity;

        //point of breadcrumbs
        std::vector<crumb>* breadcrumbs;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 600), "SFML works!");
    sf::Texture texture;
    // load a 32x32 rectangle that starts at (10, 10)
    if (!texture.loadFromFile("boid.png"))
    {
        // error...
    }

    std::vector<crumb> breadcrumbs = std::vector<crumb>();
    for(int i = 0; i < 10; i++)
    {
        crumb c(i);
        breadcrumbs.push_back(c);
    }

    boid b = boid(&window, texture, &breadcrumbs);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();                
        }

        window.clear(sf::Color(255,255,255,255));        
        b.move();
        for(int i = 0; i < breadcrumbs.size(); i++)
        {
            breadcrumbs[i].draw(&window);
        }
        b.draw();
        window.display();
    }


    return 0;
}