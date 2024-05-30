# Overview
This was a semester-long project I completed individually during my third year of college. It demonstrates a variety of AI techniques in a game enviornment.
The ultimate goal was to create realistic AI for a "player" and "monster" character, as well as being able to learn behavior from observed data.
On the way, I successfully implemented a "Flocking" behavior using AI steering behaviors as well as pathfinding, environment localization and quantization.
This project was entirely implemented and written by me. I utilized Ian Millington's book "AI for GAMES" in the following ways: Theory, mathematical formulas (entropy, information gain), and bits of pseudocode.

# Steering
I implemented a variety of steering behaviors that use a variety of mathematical principles to create realistic and smooth movement for a character.
See the "Steering" folder. The following lists each subfolder and what it demonstrates.

## Arrive/Align
This folder demonstrates the Arrive and Align behavior. After selecting a target destination, the character will
smoothly orient to face the destination as well as move towards it. It does this by assigning various paramaters, such as
radius of deceleration, radius of satisfaction, max velocity, max acceleration, and several others to determine a smooth transition.

## Velocity
This folder demonstrates the velocity matching behavior. This one is fairly straightforward. Click two places on the environment.
Depending Those two samples are taken and a velocity is estimated. Then, the character will match that velocity over the course of a second or two.
No complex trig here.

## Wander
The wander behavior uses arrive and allign and causes the character to "wander" through the play area. This behavior focuses on moving in the
direction of current movement. It gets it's new position by taking a random destination in a cone radius in front of it, and slowly navigating there.
Or, by assigning an offset each iteration to work towards a smaller goal. This is why there are two wander folders. It uses a lot of trig and also random binomials to accomplish this.

## Flocking
The magnum opus of this section, flocking is a very complex steering behavior that involves the coordination of many characters.
It utilizies velocity matching, the random binomials from wander, align, as well as two new steering behaviors; separation and cohesion.
Separation involves maintaining a distance away from characters in general. Cohesion involves moving closer to characters in front of you (in a certain radius).

# Pathfinding

## Dijkstra's algorithm
I implemented Dijkstra's algorithm in C++ mostly using maps and lists of iterators for direct, O(1) access to map locations.
I could have made it faster had I used other algorithms, but for my purposes, this was suitable. Earn your complexity.


## A* Algorithm
I implemented A* in C++ the same way. I used the euclidean and manhattan heuristics. The cluster heuristic was used, but deemed inefficient
due to the way the graph was generated.

## Generators
I created a generator to create the large graphs that I used for testing my algorithms for speed. It generates a connected graph with hundreds of thousands of 
vertices and several times that number of edges.

## Level Example
![image](https://github.com/Jeremiahk2/GameAI/assets/103688789/304f154a-5ffb-4841-a856-3158f305acdb)

I created this example using SFML to test pathfinding, learning algorithms, and so on. The red tiles are walls, and the cyan tiles are traversible space.

## Localization and Quantization

I used a tiling system to convert the environment into a graph. One tile is about the height and width of the player (The boid). Each vertex is at the center of a tile.
Edges were generated in a similar way; by connecting each tile to an adjacent tile, so long as there were no walls in the way. 
Unfortunately, SFML does not support ray-casting.

# Trees and Learning
## Trees

To implement decision trees and behavior trees, I had my character operate on a decision tree and then created a new character (a "monster") using a behavior tree.
The monster's behavior was to patrol the top part of the screen, and then pathfind to the character when the character came into range. Unless the character got too far away.
The character's behavior was to wander around pseudo-randomly. Certain rooms created patterns, such as "In room one? Go room nine" and so on. 
These rooms were located in the north part, the idea being that the character would go and grab something in those rooms and then run away and hide in a far-away room.
There were other decisions for both characters such as what happens when the monster reaches the character, if the character gets away, etc.

## Learning

The culmination of everything in the project.
I implemented a learning algorithm that would take outputted observations (in a binary text file), and convert that into a tree.
In my implementation, it takes the outputted behavior from the monster's behavior tree and converts it into a decision tree that produces near-identical behavior.
The algorithm takes data and uses various mathematical principels to construct the best decision tree it can come up with, using principles like entropy and information gain
in order to determine the best order to add decision nodes in. More data was always an improvement. My end result ended up recording and using more than ten unique observation variables and more than
100,000 lines of information per run. Granted, if behavior didn't have any randomness to it, a smaller sample size would be sufficient. But that would be a poor enemy AI.
