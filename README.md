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

## Generators

## Level Example


CSC 484 Homework Two.

Instructions and notes by part:

Part one:

1) Navigate to the HW2/P1 directory.
2) type make into the command line
3) type ./main into the command line
4) When you click, velocity is updated. For example if you click in a location and then click very quickly to the right, the character will move quickly to the right.
5) I suggest maneuvering the character by slowly moving the mouse and clicking rapidly to get a good idea of the velocity matching. Fast clicks over a large distance will speed up the boid dramatically.

Part Two:

1) Navigate to the HW2/P2 directory.
2) Type make into the command line
3) type ./main into the command line
4) Click where you want the character to arrive and align to. Probably start with the center of the screen.
5) Repeat step four.

Part Three:

There are two folders for part three. For my first implementation, with the delayed wander target update that was accidental, but still works, do the following:

1) Navigate to the HW2/P3Alt directory
2) type make into the command line
3) type ./main into the command line
4) Watch the boid move around. This is happening due to the target being assigned every second or so.

For the better wander implementation, that increases/decreases the wander target every iteration rather than assigning it every second, do the following:

1) Navigate to the HW2/P3 directory
2) Type make into the command line
3) Type ./main into the command line
4) Watch the boid move around. This is happening because the target is being updated every iteration.

Part Four:

This will look a bit weird at the start, but I promise it works.

1) Navigate to the HW2/P4 directory
2) Type make into the command line
3) Type ./main into the command line.
4) as the window pops up, you will see the boids moving around the screen very fast, this is how I randomize them.
5) After 5-10 seconds, the boids will start to slow down and form groups.
6) After that, you can really see the flocking happening.
7) Keep watching as long as you want. Notice how the flocks form and then break apart.

For P1, P2, and P3Alt, if you want to change the parameters, change the #define constants in SteeringBehavior.cpp and run make again.

For P3 and P4, if you want to change the parameters, change the class constant variables in SteeringBehavior.h and run "make remake" to clean and then compile again.
