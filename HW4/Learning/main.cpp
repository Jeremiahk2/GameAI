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
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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

#define NUM_PARAMS 8

struct Example {
    std::vector<bool> attributes;
    std::string action;
};
std::string conditions[] = {"nearPlayer", "inRoomThree", "inRoomOne", "atPlayer", "farFromSpawn", "toThree", "toOne", "toPlayer"};

void printInOrder(DecisionTreeNode *node) {
    if (node == NULL) {
        return;
    }
    bool isDecision = false;
    for (int i = 0; i < NUM_PARAMS; i++) {
        if (conditions[i] == node->name) {
            isDecision = true;
        }
    }
    if (isDecision) {
        Decision *newNode = (Decision *)node;
        std::cout << "Decision " << newNode->name << " False Node: " << std::endl;
        printInOrder(newNode->falseNode.get());

        std::cout << "Decision " << newNode->name << " True Node: " << std::endl;
        printInOrder(newNode->trueNode.get());
    }
    else {
        std::cout << "Action " << node->name << std::endl;
    }
}

float entropy(std::vector<Example> examples) {
    int exampleCount = examples.size();

    if (exampleCount <= 1) {
        return 0.f;
    }
    std::map<std::string, int> actionTallies;
    //Increment tallies.
    for (int i = 0; i < examples.size(); i++) {
        if (actionTallies.find(examples[i].action) != actionTallies.end()) {
            actionTallies.insert_or_assign(examples[i].action, actionTallies[examples[i].action] + 1);
        }
        else {
            actionTallies.insert({examples[i].action, 1});
        }
    }
    int actionCount = actionTallies.size();
    //Add action here?
    if (actionCount <= 1) {
        return 0.f;
    }

    float entropy = 0.f;

    for (auto current = actionTallies.begin(); current != actionTallies.end(); ++current) {
        float proportion = (float)current->second / (float)exampleCount;
        entropy -= proportion * log2f(proportion);
    }
    return entropy;
}

//Return a list of sets sorted by the attribute. Each set will contain 2 lists, one where all examples have this attribute as FALSE and another with TRUE.
std::map<bool, std::vector<Example>> splitByAttribute(std::vector<Example> examples, int attribute) {
    std::map<bool, std::vector<Example>> sets;
    //Go through each example and sort them into the sets based on their attribute value (true/false).
    for (int i = 0; i < examples.size(); i++) {
        if (sets.find(examples.at(i).attributes.at(attribute)) != sets.end()) {
            sets[examples.at(i).attributes.at(attribute)].push_back(examples.at(i));
        }
        else {
            sets.insert({examples.at(i).attributes.at(attribute), std::vector<Example>()});
            sets[examples.at(i).attributes.at(attribute)].push_back(examples.at(i));
        }
    }
    return sets;
}

float entropyOfSets(std::map<bool, std::vector<Example>> sets, int exampleCount) {
    float rtnEntropy = 0.f;
    //MATH
    for (auto current = sets.begin(); current != sets.end(); ++current) {
        float proportion = (float)current->second.size() / (float)exampleCount;
        rtnEntropy -= proportion * entropy(current->second);
    }
    return rtnEntropy;
}

void makeTree(std::vector<Example> examples, const std::map<int, bool *> attributes, std::shared_ptr<DecisionTreeNode> *node) {
    //Find the initial entropy of all of the examples. In other words, find the amount of information that can be gained.
    float initialEntropy = entropy(examples);
    //If there's no more information to be gained, exit.
    if (initialEntropy <= 0) {
        node->reset(new Action(examples[0].action));
        (*node)->name = examples[0].action;
        return;
    }
    if (attributes.size() == 0) {
        node->reset(new Action(examples[0].action));
        (*node)->name = examples[0].action;
        return;
    }

    int exampleCount = examples.size();
    //Set up best information.
    float bestInformationGain = 0.f;
    int bestSplitAttribute;
    std::map<bool, std::vector<Example>> bestSets;

    //Loop through each attribute
    for (auto current = attributes.begin(); current != attributes.end(); ++current) {
        //Perform the split. Returns two sets, one set has only examples with "true" for this attribute, and other with false.
        std::map<bool, std::vector<Example>> sets = splitByAttribute(examples, current->first);
        //Find overall entropy and information gain.
        float overallEntropy = entropyOfSets(sets, exampleCount);
        float informationGain = initialEntropy - overallEntropy;

        //Check if we've got the best so far.
        if (informationGain > bestInformationGain) {
            bestInformationGain = informationGain;
            bestSplitAttribute = current->first;
            bestSets = sets;
        }
    }
    node->reset(new Decision());
    (*node)->name = conditions[bestSplitAttribute];
    Decision *decisionNode = (Decision *)node->get();
    //set up the node with the best attribute.
    decisionNode->equivalence->type = GameValue::BOOLEAN;
    decisionNode->value->type = GameValue::BOOLEAN;
    decisionNode->value->data.boolean = attributes.at(bestSplitAttribute);

    std::map<int, bool *> newAttributes = attributes;
    //Erase the attribute we've just used.
    newAttributes.erase(bestSplitAttribute);
    //Go through the best sets (the true-false for the best attribute).
    for (auto current = bestSets.begin(); current != bestSets.end(); ++current) {
        //The current attribute value, true or false.
        bool attributeValue = current->first;
        //If the current attribute is true, this daughter is the true
        if (attributeValue) {
            makeTree(current->second, newAttributes, &decisionNode->trueNode);
        }
        else {
            makeTree(current->second, newAttributes, &decisionNode->falseNode);
        }
        //Make that daughter node. Each daughter node will now have a unique set attached to them.
        //But the attribute list will be the same, now with the bestSplitAttribute removed.
    }
}

int main() {

    srand(time(0));

    std::vector<Example> examples;

    std::ifstream infile("input.txt");
    std::string lineString;
    infile >> lineString;
    infile >> lineString;
    while (lineString != std::string("End")) {
        std::stringstream data(lineString);
        Example currentExample;
        for (int i = 0; i < NUM_PARAMS; i++) {
            std::string stringValue;
            getline(data, stringValue, ',');
            bool value;
            std::istringstream(stringValue) >> value;
            currentExample.attributes.push_back(value);
        }
        std::string stringValue;
        getline(data, stringValue, ',');
        currentExample.action = stringValue;
        examples.push_back(currentExample);
        infile >> lineString;
    }

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
    //nearPlayer,inRoomThree,inRoomOne,atPlayer,farFromSpawn,toThree,toOne,toPlayer
    int currentDistToPlayer = 6;
    bool nearPlayer = false;
    bool isMonsterRoomThree = false;
    bool isMonsterRoomOne = false;
    bool atPlayer = b.sprite.getGlobalBounds().intersects(monster.sprite.getGlobalBounds());
    bool farFromSpawn = false;
    bool toThree = false;
    bool toOne = false;
    bool toPlayer = false;
    //Create behavior tree.
    std::shared_ptr<DecisionTreeNode> rootPointer;
    std::map<int, bool *> attributes;

    attributes.insert({0, &nearPlayer});
    attributes.insert({1, &isMonsterRoomThree});
    attributes.insert({2, &isMonsterRoomOne});
    attributes.insert({3, &atPlayer});
    attributes.insert({4, &farFromSpawn});
    attributes.insert({5, &toThree});
    attributes.insert({6, &toOne});
    attributes.insert({7, &toPlayer});

    std::cout << &nearPlayer << std::endl;
    std::cout << &isMonsterRoomThree << std::endl;
    std::cout << &isMonsterRoomOne << std::endl;
    std::cout << &atPlayer << std::endl;
    std::cout << &farFromSpawn << std::endl;
    std::cout << &toThree << std::endl;
    std::cout << &toOne << std::endl;
    std::cout << &toPlayer << std::endl;

    std::cout << "Running" << std::endl;
    makeTree(examples, attributes, &rootPointer);
    std::cout << "Done running" << std::endl;

    std::cout << (rootPointer.get() == NULL) << std::endl;
    
    printInOrder(rootPointer.get());


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