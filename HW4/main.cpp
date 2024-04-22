#include "Timeline.h"
#include "SteeringBehavior.h"
#include "Pathfinding.h"
#include "DecisionTree.h"
#include "BehaviorTree.h"

#include <cstdio>
#include <deque>
#include <iostream>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <fstream>

//The tile number for the tile in the cetner of room one.
#define ROOM_ONE 108
//The tile number for the tile in the center of room three
#define ROOM_THREE 784
//The tile number for a tile in room six.
#define ROOM_SIX 817
//The tile number for a tile in room nine.
#define ROOM_NINE 801
//The range at which the monster detects the player.
#define AGGRO_RANGE 10
//The range at which the monster will stop tracking the player.
#define SPAWN_DISTANCE 20

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
    Boid monster(&window, texture);
    monster.kinematic.pos = sf::Vector2f(50.f, 50.f);
    monster.sprite.setPosition(monster.kinematic.pos);
    SteeringBehavior::boids.push_back(&b);
    SteeringBehavior::boids.push_back(&monster);
    //Set up clickCircles 
    std::deque<sf::CircleShape> clickCircles;

    //Set up environment variables
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

    //Set up visual environment
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
            //Draw couch
            if (j == bottomWall - 5) {
                if (i == midLeftWall + 7 || i == midLeftWall + 10) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            if (j == bottomWall - 4) {
                if (i == midLeftWall + 7 || i == midLeftWall + 8 || i == midLeftWall + 9 || i == midLeftWall + 9 || i == midLeftWall + 10) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            //Draw lift
            if (j == midTopWall + 2 || j == midBottomWall - 2) {
                if (i == midRightWall + 2 || i == midRightWall + 3 || i == midRightWall + 4 || i == midRightWall + 5 || i == midRightWall + 6) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            if (i == rightWall - 2) {
                if (j == midBottomWall - 3 || j == midBottomWall - 4 || j == midBottomWall - 5 || j == midBottomWall - 6) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            if (i == midRightWall + 2) {
                if (j == midBottomWall - 3 || j == midTopWall + 3) {
                    tile.setFillColor(sf::Color::Red);
                }
            }

            //Draw stairs
            if (j == midTopWall + 1) {
                if (i == midLeftWall - 3) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            if (j == midTopWall + 2) {
                if (i == midLeftWall - 4) {
                    tile.setFillColor(sf::Color::Red);
                }
            }
            if (j == midTopWall + 3) {
                if (i == midLeftWall - 5) {
                    tile.setFillColor(sf::Color::Red);
                }
            }

            
            // tile.setOutlineThickness(1.f);
            // tile.setOutlineColor(sf::Color::Black);
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
                std::shared_ptr<Edge::Vertex> vertex(new Edge::Vertex(sf::Vector2f(-1.f, -1.f)));
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
    
    //Initialize decision tree test case variables
    //Initialize game state variables.
    bool atDestination = true;
    bool isCharRoomOne = false;
    bool isCharRoomTwo = false;
    //Initialize decision tree nodes
    Action *changeSix = new Action("ChangeSix");
    Action *changeNine = new Action("ChangeNine");
    Action *changeRandom = new Action("ChangeRandom");
    Action *followPath = new Action("FollowPath");

    Decision *root = new Decision();
    root->equivalence->type = GameValue::BOOLEAN;
    root->value->type = GameValue::BOOLEAN;
    root->value->data.boolean = &atDestination;

    Decision *roomOneDecision = new Decision();
    roomOneDecision->equivalence->type = GameValue::BOOLEAN;
    roomOneDecision->value->type = GameValue::BOOLEAN;
    roomOneDecision->value->data.boolean = &isCharRoomOne;

    Decision *roomTwoDecision = new Decision();
    roomTwoDecision->equivalence->type = GameValue::BOOLEAN;
    roomTwoDecision->value->type = GameValue::BOOLEAN;
    roomTwoDecision->value->data.boolean = &isCharRoomTwo;

    root->trueNode.reset(roomOneDecision);
    root->falseNode.reset(followPath);
    roomOneDecision->trueNode.reset(changeSix);
    roomOneDecision->falseNode.reset(roomTwoDecision);
    roomTwoDecision->trueNode.reset(changeNine);
    roomTwoDecision->falseNode.reset(changeRandom);

    //Initialize any extra game state variables for the behavior tree.
    int currentDistToPlayer = 6;
    bool isMonsterRoomThree = false;
    bool isMonsterRoomOne = false;
    bool atPlayer = b.sprite.getGlobalBounds().intersects(monster.sprite.getGlobalBounds());
    bool farFromSpawn = false;
    //Save state for waiting actions.
    int savedVariableDist = currentDistToPlayer;
    bool savedMonsterThree = isMonsterRoomThree;
    bool savedMonsterOne = isMonsterRoomOne;
    bool savedAtPlayer = atPlayer;
    bool savedFarFromSpawn = farFromSpawn;
    bool saved = false;
    //Initialize any test case variables for the behavior tree.
    int minDistToPlayer = AGGRO_RANGE;
    //Initialize behavior tree nodes.
    ActionTask *goPlayer = new ActionTask("goPlayer");
    ActionTask *killPlayer = new ActionTask("killPlayer");
    std::shared_ptr<ActionTask> goThree(new ActionTask("goThree"));
    std::shared_ptr<ActionTask> goOne(new ActionTask("goOne"));
    

    Condition *nearPlayerCondition = new Condition();
    nearPlayerCondition->upperBound->type = GameValue::NUMBER;
    nearPlayerCondition->upperBound->data.number = &minDistToPlayer;
    nearPlayerCondition->value->type = GameValue::NUMBER;
    nearPlayerCondition->value->data.number = &currentDistToPlayer;
    Condition *roomOneCondition = new Condition();
    roomOneCondition->equivalence->type = GameValue::BOOLEAN;
    roomOneCondition->value->type = GameValue::BOOLEAN;
    roomOneCondition->value->data.boolean = &isMonsterRoomOne;
    Condition *roomThreeCondition = new Condition();
    roomThreeCondition->equivalence->type = GameValue::BOOLEAN;
    roomThreeCondition->value->type = GameValue::BOOLEAN;
    roomThreeCondition->value->data.boolean = &isMonsterRoomThree;
    
    Sequence *nearPlayerSequence = new Sequence();
    nearPlayerSequence->addChild(std::shared_ptr<BehaviorTreeNode>(nearPlayerCondition));
    nearPlayerSequence->addChild(std::shared_ptr<BehaviorTreeNode>(goPlayer));
    nearPlayerSequence->addChild(std::shared_ptr<BehaviorTreeNode>(killPlayer));
    Sequence *roomOneSequence = new Sequence();
    roomOneSequence->addChild(std::shared_ptr<BehaviorTreeNode>(roomOneCondition));
    roomOneSequence->addChild(goThree);
    Sequence *roomThreeSequence = new Sequence();
    roomThreeSequence->addChild(std::shared_ptr<BehaviorTreeNode>(roomThreeCondition));
    roomThreeSequence->addChild(goOne);
    RandomSelector *randomRoomSelector = new RandomSelector();
    randomRoomSelector->addChild(goOne);
    randomRoomSelector->addChild(goThree);
    Selector *rootSelector = new Selector();
    rootSelector->addChild(std::shared_ptr<BehaviorTreeNode>(nearPlayerSequence));
    rootSelector->addChild(std::shared_ptr<BehaviorTreeNode>(roomOneSequence));
    rootSelector->addChild(std::shared_ptr<BehaviorTreeNode>(roomThreeSequence));
    rootSelector->addChild(std::shared_ptr<BehaviorTreeNode>(randomRoomSelector));

    BehaviorTree behaviorTree = BehaviorTree(std::shared_ptr<BehaviorTreeNode>(rootSelector));

    //Set up the output file for the behavior tree.
    std::ofstream output ("output.txt", std::ofstream::out);
    output << "nearPlayer,inRoomThree,inRoomOne,atPlayer" << std::endl; //Set up key line.

    // Set up steering behaviors.
    Kinematic target;
    target.pos = b.kinematic.pos;

    Pathfollowing pathFollower;
    std::deque<std::shared_ptr<Edge::Vertex>> path;

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
                    int targetTileX = floor(target.pos.x / tileSize);
                    int targetTileY = floor(target.pos.y / tileSize);
                    std::cout << "Target Tile X: " << targetTileX << std::endl;
                    std::cout << "Target Tile Y: " << targetTileY << std::endl;
                    std::cout << "Tile number: " << targetTileX * verticalTiles + targetTileY << std::endl;

                    sf::CircleShape c;
                    c.setRadius(2.5);
                    c.setOrigin(2.5, 2.5);
                    c.setFillColor(sf::Color::Green);
                    c.setPosition(target.pos);
                    clickCircles.push_back(c);
                    if (clickCircles.size() > 10) {
                        clickCircles.pop_front();
                    }
                }
            }
            //Update state variables.
            int playerTileX = floor(b.kinematic.pos.x / tileSize);
            int playerTileY = floor(b.kinematic.pos.y / tileSize);
            int monsterTileX = floor(monster.kinematic.pos.x / tileSize);
            int monsterTileY = floor(monster.kinematic.pos.y / tileSize);
            atDestination = path.size() == 0 || fillers[playerTileX * verticalTiles + playerTileY]->position == path[path.size() - 1]->position ? true : false; //Update player destination status.
            isCharRoomOne = (playerTileX >= 1 && playerTileX <= 7) && (playerTileY >= 1 && playerTileY <= 7) ? true : false; //Update room one status for character
            isCharRoomTwo = (playerTileX >= 9 && playerTileX <= 25) && (playerTileY >= 1 && playerTileY <= 7) ? true : false; //Update room two status for character
            isMonsterRoomOne = ( monsterTileX >= 1 && monsterTileX <= 7) && (monsterTileY >= 1 && monsterTileY <= 7) ? true : false; //Update room two status for monster
            isMonsterRoomThree = ( monsterTileX >= 27 && monsterTileX <= 33) && (monsterTileY >= 1 && monsterTileY <= 7) ? true : false; //Update room three status monster
            atPlayer = b.sprite.getGlobalBounds().intersects(monster.sprite.getGlobalBounds()); //Update character kill condition

            std::deque<std::shared_ptr<Edge::Vertex>> pathToPlayer;
            std::deque<std::shared_ptr<Edge::Vertex>> pathToSpawn;
            Pathfinding astar;
            pathToPlayer = astar.calculateAStar(graph, fillers[monsterTileX * verticalTiles + monsterTileY], fillers[playerTileX * verticalTiles + playerTileY]);
            for (int i = 0; i < graph.vertices.size(); i++) {
                graph.vertices[i]->visited = false;
            }
            Pathfinding otherAstar;
            pathToSpawn = otherAstar.calculateAStar(graph, fillers[playerTileX * verticalTiles + playerTileY], fillers[ROOM_ONE]);
            for (int i = 0; i < graph.vertices.size(); i++) {
                graph.vertices[i]->visited = false;
            }
            farFromSpawn = pathToSpawn.size() < SPAWN_DISTANCE ? false : true; //If the player is closer than SPAWN_DISTANCE, it's not too far from spawn.
            currentDistToPlayer = pathToPlayer.size(); //Update distance from monster to player.


            //Run character decision tree.
            root->makeDecision();
            behaviorTree.runTree();
            if (BehaviorTreeNode::actionQueue.size() == 1) {
                if (BehaviorTreeNode::actionQueue.begin()->second == STATUS::WAITING && !saved) {
                    std::cout << "Saving" << std::endl;
                    savedMonsterOne = isMonsterRoomOne;
                    savedMonsterThree = isMonsterRoomThree;
                    savedVariableDist = currentDistToPlayer;
                    savedAtPlayer = atPlayer;
                    savedFarFromSpawn = farFromSpawn;
                    saved = true;
                }
                output << (savedVariableDist < AGGRO_RANGE) << "," << savedMonsterThree << "," << savedMonsterOne << "," << savedAtPlayer << "," << farFromSpawn << "," << BehaviorTreeNode::actionQueue.begin()->first << std::endl;
                std::cout <<  (savedVariableDist < AGGRO_RANGE) << "," << savedMonsterThree << "," << savedMonsterOne << "," << savedAtPlayer << "," << farFromSpawn << "," << BehaviorTreeNode::actionQueue.begin()->first << std::endl;
            }
            //Handle actions in the character's queue.
            for (std::string current : DecisionTreeNode::actionQueue) {
                if (current == "ChangeRandom") {
                    std::cout << "ChangeRandom" << std::endl;
                    int boidTileX = floor(b.kinematic.pos.x / tileSize);
                    int boidTileY = floor(b.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> boidVertex = fillers[boidTileX * verticalTiles + boidTileY];
                    //Set target as the center of the screen.
                    std::shared_ptr<Edge::Vertex> targetVertex = graph.vertices[rand() % graph.vertices.size()];
                    //Find the new path to the target.
                    Pathfinding astar;
                    path = astar.calculateAStar(graph, boidVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }
                }
                else if (current == "FollowPath") {
                    //Calculate acceleration for character boid.
                    if (path.size() != 0) {
                        int goal = pathFollower.followPath(path, 1, frameTime.getRealTicLength() * (float)(currentTic - lastTic), b.kinematic);
                        Kinematic goalKinematic;
                        goalKinematic.pos = path[goal]->position;
                        pathFollower.calculateAcceleration(b.steering, b.kinematic, goalKinematic);
                    }
                }
                else if (current == "ChangeSix") {
                    std::cout << "ChangeSix" << std::endl;
                    int boidTileX = floor(b.kinematic.pos.x / tileSize);
                    int boidTileY = floor(b.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> boidVertex = fillers[boidTileX * verticalTiles + boidTileY];
                    //Set target as the center of the screen.
                    std::shared_ptr<Edge::Vertex> targetVertex = fillers[ROOM_SIX];
                    //Find the new path to the target.
                    Pathfinding astar;
                    path = astar.calculateAStar(graph, boidVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }
                }
                else if (current == "ChangeNine") {
                    std::cout << "ChangeNine" << std::endl;
                    int boidTileX = floor(b.kinematic.pos.x / tileSize);
                    int boidTileY = floor(b.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> boidVertex = fillers[boidTileX * verticalTiles + boidTileY];
                    //Set target as the center of the screen.
                    std::shared_ptr<Edge::Vertex> targetVertex = fillers[ROOM_NINE];
                    //Find the new path to the target.
                    Pathfinding astar;
                    path = astar.calculateAStar(graph, boidVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }
                }
            }

            
            //Handle actions in the monster's queue.
            for (auto current = BehaviorTreeNode::actionQueue.begin(); current != BehaviorTreeNode::actionQueue.end(); ++current) {
                if (current->first == "goPlayer") {
                    // output << "goPlayer" << std::endl;
                    //Calculate acceleration for monster boid.
                    if (pathToPlayer.size() > AGGRO_RANGE) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goPlayer", STATUS::FAILURE);
                    }
                    else if (pathToPlayer.size() != 0) {
                        int goal = pathFollower.followPath(pathToPlayer, 1, frameTime.getRealTicLength() * (float)(currentTic - lastTic), monster.kinematic);
                        Kinematic goalKinematic;
                        goalKinematic.pos = pathToPlayer[goal]->position;
                        pathFollower.calculateAcceleration(monster.steering, monster.kinematic, goalKinematic);
                    }
                    if (farFromSpawn) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goPlayer", STATUS::FAILURE);
                    }
                    if (atPlayer) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goPlayer", STATUS::SUCCESS);
                    }
                }
                else if (current->first == "killPlayer") {
                    std::cout << atPlayer << std::endl;
                    // output << "killPlayer" << std::endl;
                    b.kinematic.pos = sf::Vector2f(200.f, 200.f);
                    atDestination = true;
                    path.clear();
                    monster.kinematic.pos = sf::Vector2f(50.f, 50.f);
                    BehaviorTreeNode::actionQueue.insert_or_assign("killPlayer", STATUS::SUCCESS);
                }
                else if (current->first == "goOne") {
                    // output << "goOne" << std::endl;
                    
                    std::deque<std::shared_ptr<Edge::Vertex>> pathToOne;
                    int monsterTileX = floor(monster.kinematic.pos.x / tileSize);
                    int monsterTileY = floor(monster.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> monsterVertex = fillers[monsterTileX * verticalTiles + monsterTileY];
                    //Set target as the center of the screen.
                    std::shared_ptr<Edge::Vertex> targetVertex = fillers[ROOM_ONE];
                    //Find the new path to the target.
                    Pathfinding astar;
                    pathToOne = astar.calculateAStar(graph, monsterVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }

                    if (pathToOne.size() != 0) {
                        int goal = pathFollower.followPath(pathToOne, 1, frameTime.getRealTicLength() * (float)(currentTic - lastTic), monster.kinematic);
                        Kinematic goalKinematic;
                        goalKinematic.pos = pathToOne[goal]->position;
                        pathFollower.calculateAcceleration(monster.steering, monster.kinematic, goalKinematic);
                    }
                    if (currentDistToPlayer < minDistToPlayer && !farFromSpawn) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goOne", STATUS::FAILURE);
                    }
                    else if (monsterVertex == targetVertex) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goOne", STATUS::SUCCESS);
                    }
                }
                else if (current->first == "goThree") {
                    // output << "goThree" << std::endl;

                    std::deque<std::shared_ptr<Edge::Vertex>> pathToThree;
                    int monsterTileX = floor(monster.kinematic.pos.x / tileSize);
                    int monsterTileY = floor(monster.kinematic.pos.y / tileSize);
                    std::shared_ptr<Edge::Vertex> monsterVertex = fillers[monsterTileX * verticalTiles + monsterTileY];
                    //Set target as the center of the screen.
                    std::shared_ptr<Edge::Vertex> targetVertex = fillers[ROOM_THREE];
                    //Find the new path to the target.
                    Pathfinding astar;
                    pathToThree = astar.calculateAStar(graph, monsterVertex, targetVertex);
                    for (int i = 0; i < graph.vertices.size(); i++) {
                        graph.vertices[i]->visited = false;
                    }

                    if (pathToThree.size() != 0) {
                        int goal = pathFollower.followPath(pathToThree, 1, frameTime.getRealTicLength() * (float)(currentTic - lastTic), monster.kinematic);
                        Kinematic goalKinematic;
                        goalKinematic.pos = pathToThree[goal]->position;
                        pathFollower.calculateAcceleration(monster.steering, monster.kinematic, goalKinematic);
                    }
                    if (currentDistToPlayer < minDistToPlayer && !farFromSpawn) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goThree", STATUS::FAILURE);
                    }
                    else if (monsterVertex == targetVertex) {
                        BehaviorTreeNode::actionQueue.insert_or_assign("goThree", STATUS::SUCCESS);
                    }
                }
            }  
            if (BehaviorTreeNode::actionQueue.size() == 1) {
                if (BehaviorTreeNode::actionQueue.begin()->second != STATUS::WAITING) {
                    std::cout << "UnSaving" << std::endl;
                    saved = false;
                }
            }
            //Clear the action queue.
            DecisionTreeNode::actionQueue.clear();

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