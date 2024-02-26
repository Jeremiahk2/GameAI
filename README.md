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
