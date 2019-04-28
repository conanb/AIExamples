The folder 'aiToolkit' contains code used within multiple projects.

Agents: 
- Simple entity class and Behaviour component class that has an execute() method that returns true if the behaviour succeeded, or false. The entity has a std::vector of Behaviours and in the entity's update() method it would iterate over the components and pass itself and delta time into the execute() method. Includes an example Behaviour that would move a entity towards a target entity at a set speed, and another Behaviour that would use the keyboard to move the target entity. So the app would have an entity the player controls, and another that followed them.
  
Finite State Machines:
- Created a FSM Behaviour, State class, Transition class and a Condition class. FSM update() would query the current State's Transitions to see if their Condition has been met, and if so would transition to the new State. The State would simply have an update() method similar to a Behaviour, but has a collection of Transitions. A Transition simply had a target State and a collection of Condition. Condition would simply have a function that returned true or false. This allowed separation of logic from data. The example then recreated the first Agents example, but with a FSM for the chasing entity. Gave it an idle state when the player was out of range, which transitions to chase when they were close. The player was faster than the entity to make the example more interesting.
  
Steering Behaviours:
- Created a SteeringBehaviour Behaviour, and SteeringForce class. SteeringForce implemented a single steering behaviour (seek or flee etc) and the SteeringBehaviour would just store a force to use. Example just used Seek, Flee and Wander set up with a FSM. Wander when player not near, Seek when they are, then added another entity that used Flee instead of Seek when the player was close. Added Evade and Pursue, and Weighted Priority Running Sum to the SteeringBehaviour component so that it could run multiple forces together.
  
Blackboards:
- Set up a Blackboard class and gave entity's a Blackboard that could then store all variables they used (position xy, speed, etc). Also set up Blackboard to use Expert/Arbiters.

Pathfinding:
- Dijkstras and A*. Search class that just has static member functions. "Waypoints" are generic Node classes without any position etc, they just have a collection of Link classes that have costs and target Node (one-way links). Also made a dijkstrasToClosestFlag() function and gave Node a bitfield of flags (unsigned int).

Decision Trees:
- Recreated the FSM example using Decision Trees. Created an abstract Decision class that had a makeDecision() method that used a entity* and delta time as parameters. A derived ConditionalDecision would have a true and false Decision, and a Condition. On Condition true the true Decision was executed. On False the false Decision. Created a DecisionBehaviour that simple had a Decision that it executed. Added some other bits like a RandomDecision that would hold a std::vector of Decisions and randomly execute one when it is executed.
  
Behaviour Trees:
- Recreated the Decision Tree example but using Behaviour Trees. Since Behaviour already laid the ground-work for Behaviour Trees I just had to make a CompositeBehaviour class, then a SequenceBehaviour and SelectorBehaviour. Added other Composites such as Switch (used an enum to select the child Behaviour to run), RandomSwitch and then Decorators such as Not, Timeout and Log (logs a message when it executes and then executes the child behaviour). Changed the original Behaviour class to return an enum rather than true/false so that Behaviour Trees could return "ongoing".   
Flocking:
- SteeringForce's to implement Flocking behaviours.
  
Flow Fields:
- Implemention of a Flow Field and then a FlowFieldForce to use for SteeringBehaviours.
  
Navigation Meshes:
- Nav Mesh which derived from Pathfinding code so that the original Dijkstras/A* would work with it unchanged.
  
Planners:
- Simple dock-worker-robot problem solver using STRIPS.
  
Game Trees - MiniMax:
- MiniMax implemented for tic-tac-toe and connect-four.
- MCTS implemented for tic-tac-toe and connect-four.