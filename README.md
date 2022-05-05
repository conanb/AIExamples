# AI Examples
AI Examples is a collection of C++ game AI examples that I had developed and taught at the Academy of Interactive Entertainment for the Artificial Intelligence subject.

## Build

Ensure you have first cloned the repository recursively to include all sub-modules:

```
git clone https://github.com/conanb/AIExamples.git --recursive
```

The examples are built using CMake. Simply run the following command on the root directory:

```
cmake ./
```

Examples have been tested on Windows 7/10/11 using Microsoft Visual Studio 2019/2022.

## Repository Contents
The repository is broken into 4 parts: third-party dependencies, app toolkit, ai toolkit, and examples. The examples demonstrate systems from the AI Toolkit using the App Toolkit for visualisation purposes.

### App Toolkit
App toolkit consists of a basic 2D OpenGL renderer and application framework using GLFW. All examples make use of this toolkit.

### AI Toolkit
AI Toolkit contains all of the code for the different ai systems that were taught in the subject. An overview of the systems included, and their examples, are:

Agents: 
- Simple entity class and Behaviour component class that has an execute() method that returns true if the behaviour succeeded, or false. The entity has a std::vector of Behaviours and in the entity's update() method it would iterate over the components and pass itself and time-delta into the execute() method. Includes an example Behaviour that would move a entity towards a target entity at a set speed, and another Behaviour that would use the keyboard to move the target entity. So the app would have an entity the player controls, and another that followed them.
  
Finite State Machines:
- FSM implemented as a Behaviour, with a State class, Transition class and a Condition class. FSM update() would query the current State's Transitions to see if their Condition has been met, and if so would transition to the new State. The State would simply have an update() method similar to a Behaviour, but has a collection of Transitions. A Transition simply had a target State and a collection of Condition. Condition would simply have a function that returned true or false. This allowed separation of logic from data. The example then recreated the first Agents example, but with a FSM for the chasing entity. Gave it an idle state when the player was out of range, which transitions to chase when they were close. The player was faster than the entity to make the example more interesting.
  
Steering Behaviours:
- Steering implemented as a Behaviour componenent with a SteeringForce class. SteeringForce implemented a single steering behaviour (seek or flee etc) and the SteeringBehaviour would just store a force to use. Example demonstrates Seek, Flee and Wander set up with a FSM. Agents Wander when player not near, Seek when they are, or alternatively Flee instead of Seek when the player was close. Added Evade and Pursue, and Weighted Priority Running Sum to the SteeringBehaviour component so that it could run multiple forces together.
  
Blackboards:
- Implements a Blackboard class and gave entity's a Blackboard that could then store all variables they used (position xy, speed, etc). Also set up Blackboard to use Expert/Arbiters pattern. Included example demonstrates the pattern using 3 types of agents who occasionally flag requests for help from specified types of other agents.

Pathfinding:
- Dijkstras and A* implemented. Search class that just has static member functions. "Waypoints" are generic Node classes without any position etc, they just have a collection of Link classes that have costs and target Node (one-way links). Also made a dijkstrasToClosestFlag() function and gave Node a bitfield of flags (unsigned int).

Decision Trees:
- Recreated the FSM example using Decision Trees. Created an abstract Decision class that had a makeDecision() method that used a entity* and time-delta as parameters. A derived ConditionalDecision would have a true and false Decision, and a Condition. On Condition true the true Decision was executed. On False the false Decision. Created a DecisionBehaviour that simple had a Decision that it executed. Added some other bits like a RandomDecision that would hold a std::vector of Decisions and randomly execute one when it is executed.
  
Behaviour Trees:
- Recreated the Decision Tree example but using Behaviour Trees. Since the initial Behaviour component already laid the ground-work for Behaviour Trees I just had to make a CompositeBehaviour class, then a SequenceBehaviour and SelectorBehaviour. Added other Composites such as Switch (used an enum to select the child Behaviour to run), RandomSwitch and then Decorators such as Not, Timeout and Log (logs a message when it executes and then executes the child behaviour). Changed the original Behaviour class to return an enum rather than true/false so that Behaviour Trees could return "ongoing".   

Flocking:
- Specified SteeringForce's to implement Flocking behaviours.
  
Flow Fields:
- Implemention of a Flow Field and then a FlowFieldForce to use for SteeringBehaviours.
  
Navigation Meshes:
- Nav Mesh example which derived from Pathfinding code so that the original Dijkstras/A* would work with it unchanged. Trianglular mesh generated from defined area + obstacles, with path then smoothed to travel shortest distance between mesh nodes (triangles).
  
Planners:
- Simple STRIPS (Stanford Research Institute Problem Solver) implementation used to demonstrate a dock-worker-robot problem solver.
  
Game Trees - MiniMax + Monte Carlo Tree Search:
- MiniMax implemented for tic-tac-toe and connect-four.
- MCTS implemented for tic-tac-toe and connect-four.
