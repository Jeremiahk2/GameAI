CSC 484 Homework Four.

Instructions and notes by part:

Parts one and two

1) Part one and part two is included in the "Trees" directory. Navigate there in the terminal.
2) Type "make" or "make remake" into the terminal (for a clean make in case I forgot to clean first)
3) type ./main. You should see a window appear with two boids travelling around.
4) The boid trailing white circles is the character boid, using a decision tree.
5) the boid trailing magenta circles is the monster boid, using a behavior tree.
6) If you want to edit the aggro radius of the monster, or the "give up" radius of the monster, look at the constants AGGRO_RANGE and SPAWN_DISTANCE at the top of main. Though the default values are what I thought looked best. Make sure to type make afterwards if you do.
7) an attribute vector file is being generated as this plays, for optional use when testing part three.


Part Three

1) Part three is included in the "Learning" directory. Navigate there in the terminal.
2) Type "make" or "make remake" into the terminal (for a clean make in case I forgot to clean first)
3) Type ./main. A window will appear, though it may say something like "No response from window". This is expected, it's compiling data and making the learned decision tree. It only lasts 5-10 seconds for me.
4) The window will appear, showing two boids. The pattern is the same as for part one and two in terms of which boid is which and how to adjust aggression from the enemy. 
5) A sample input file with 150,000 examples has been provided, so you do not need to go back to the Trees and generate your own if you don't want to. This is the file I got the best results out of.
6) If you want to swap input files, move "input-4.txt" elsewhere and bring your own in. Then rename yours to "input-4.txt".
