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

    //Set up environment
    std::deque<sf::RectangleShape> tiles;
    float tileSize = b.sprite.getGlobalBounds().width;
    int horizontalTiles = winWidth / tileSize + 1;
    int verticalTiles = winHeight / tileSize + 1;


    int leftWall = 0;
    int rightWall = horizontalTiles - 1;
    int midRightWall = horizontalTiles - 9;
    int midLeftWall = 8;
    int bottomWall = verticalTiles - 1;
    int topWall = 0;
    int midBottomWall = verticalTiles - 9;
    int midTopWall = 8;

    //Set up environment
    for (int i = 0; i < horizontalTiles; i++) {
        for (int j = 0; j < verticalTiles; j++) {
            sf::RectangleShape tile;
            tile.setSize(sf::Vector2f(tileSize, tileSize));
            tile.setPosition(i * tileSize, j * tileSize);
            tile.setFillColor(sf::Color(0, 128, 128));
            //Set up four bounding walls.
            if (j == topWall || j == bottomWall || i == leftWall || i == rightWall) {
                tile.setFillColor(sf::Color::Red);
            }
            //Set up midleft and midright wall.
            if (i == midLeftWall || i == midRightWall) {
                //Leave space for doorway on mid level.
                if (!(j == midTopWall + 4 || j == midTopWall + 5)) {
                    //Leave some space for doorway on top level
                    if (!(j == topWall + 4)) {
                        //Leave some space for doorway on botom level.
                        if (!(j == bottomWall - 3 || j == bottomWall - 4 || j == bottomWall - 5)) {
                            tile.setFillColor(sf::Color::Red);
                        }
                    }
                }
            }
            //Set up midtop and midbottom wall.
            if (j == midTopWall || j == midBottomWall) {
                //Leave space for doorway on left side.
                if (!(i == leftWall + 3 || i == leftWall + 4 || i == leftWall + 5)) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            
            tile.setOutlineThickness(1.f);
            tile.setOutlineColor(sf::Color::Black);
            tiles.push_back(tile);
        }
    }


    //Set up graph environment.
    Graph graph;
    std::deque<std::shared_ptr<Edge::Vertex>> fillers;
    sf::Color red = sf::Color::Red;
    sf::Color cyan = sf::Color(0, 128, 128);
    //Set up vertices.
    int count = 0;
    for (int i = 0; i < horizontalTiles; i++) {
        for (int j = 0; j < verticalTiles; j++) {
            //Set up vertex.
            if (tiles[i * verticalTiles + j].getFillColor() != red) {
                std::shared_ptr<Edge::Vertex> vertex(new Edge::Vertex);
                vertex->position = sf::Vector2f(i * tileSize + tileSize / 2, j * tileSize + tileSize / 2);
                vertex->id = count++;
                graph.vertices.push_back(vertex);
                fillers.push_back(vertex);
            }   
            //Filler list for easy indexing later.
            else {
                std::shared_ptr<Edge::Vertex> vertex(new Edge::Vertex);
                fillers.push_back(vertex);
            }
        }
    }
    //Set up edges
    float straightWeight = tileSize;
    float diagonalWeight = Pathfollowing::euclidean(sf::Vector2f(0.f, 0.f), sf::Vector2f(tileSize, tileSize));
    for (int i = 1; i < horizontalTiles - 1; i++) {
        for (int j = 1; j < verticalTiles - 1; j++) {
            if (tiles[i * verticalTiles + j].getFillColor() != red) {
                //Set up edges to surrounding viable neighbors.
                // I++ equals to the right. J++ equals down

                std::deque<std::pair<int, int>> valid;
                int currentI = i - 1;
                int currentJ = j;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red) {
                    valid.push_back({currentI, currentJ});
                }
                currentI = i + 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red) {
                    valid.push_back({currentI, currentJ});
                }
                currentI = i;
                currentJ = j - 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red) {
                    valid.push_back({currentI, currentJ});
                }
                currentJ = j + 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red) {
                    valid.push_back({currentI, currentJ});
                }
                currentI = i + 1;
                currentJ = j + 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI - 1) * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI) * verticalTiles + currentJ - 1].getFillColor() != red) {

                    valid.push_back({currentI, currentJ});
                }
                currentI = i - 1;
                currentJ = j + 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI + 1) * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI) * verticalTiles + currentJ - 1].getFillColor() != red) {

                    valid.push_back({currentI, currentJ});
                }
                currentI = i + 1;
                currentJ = j - 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI - 1) * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI) * verticalTiles + currentJ + 1].getFillColor() != red) {

                    valid.push_back({currentI, currentJ});
                }
                currentI = i - 1;
                currentJ = j - 1;
                if (tiles[currentI * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI + 1) * verticalTiles + currentJ].getFillColor() != red &&
                    tiles[(currentI) * verticalTiles + currentJ + 1].getFillColor() != red) {

                    valid.push_back({currentI, currentJ});
                }
                std::cout << valid.size() << std::endl;
                for (int k = 0; k < valid.size(); k++) {
                    std::shared_ptr<Edge> edge(new Edge);
                    if (valid[k].first != i && valid[k].second != j) {
                        edge->weight = diagonalWeight;
                    }
                    else {
                        edge->weight = straightWeight;
                    }
                    edge->start = fillers[i * verticalTiles + j];
                    edge->end = fillers[valid[k].first * verticalTiles + valid[k].second];
                    fillers[i * verticalTiles + j]->outgoingEdges.push_back(edge);
                    graph.edges.push_back(edge);
                }
            }   
        }
    }

    // //Show tile centers
    // for (int i = 0; i < graph.vertices.size(); i++) {
    //     sf::CircleShape c;
    //     c.setRadius(2.5);
    //     c.setOrigin(2.5, 2.5);
    //     c.setFillColor(sf::Color::Green);
    //     c.setPosition(graph.vertices[i]->position);
    //     clickCircles.push_back(c);
    // }

    // Set up steering behaviors.
    Kinematic target;
    target.pos = b.kinematic.pos;

    Pathfollowing pathFollower;

    //Create a timeline with a tic size of 10, updating every 10 ms or 100 fps.
    Timeline global;
    int tic = 10;
    Timeline frameTime(&global, tic);

    //CurrentTic starts higher than lastTic so the program starts immediately.
    int64_t currentTic = 0;
    int64_t lastTic = -1;

    std::deque<std::shared_ptr<Edge::Vertex>> path;

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
                    int targetTileX = floor(target.pos.x / tileSize);
                    int targetTileY = floor(target.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> targetVertex = fillers[targetTileX * verticalTiles + targetTileY];

                    int boidTileX = floor(b.kinematic.pos.x / tileSize);
                    int boidTileY = floor(b.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> boidVertex = fillers[boidTileX * verticalTiles + boidTileY];

                    Pathfinding astar;
                    path = astar.calculateAStar(graph, boidVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }


                }
            }

            //Calculate current path.


            //Calculate acceleration for all boids.
            for (Boid *b : SteeringBehavior::boids) {
                if (path.size() != 0) {
                    int goal = pathFollower.followPath(path, 1, frameTime.getRealTicLength() * (float)(currentTic - lastTic), b->kinematic);
                    Kinematic goalKinematic;
                    goalKinematic.pos = path[goal]->position;
                    pathFollower.calculateAcceleration(b->steering, b->kinematic, goalKinematic);
                }
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
            for (int i = 0; i < clickCircles.size(); i++) {
                window.draw(clickCircles[i]);
            }
            window.display();
        }
        

        //We've processed a tic, wait for the next one.
        lastTic = currentTic;
    }
    return EXIT_SUCCESS;
}