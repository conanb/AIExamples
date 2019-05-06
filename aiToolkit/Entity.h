#pragma once

#include <vector>

#include "Blackboard.h"

namespace ai {

class Behaviour;

class Entity {
public:

	Entity() {}
	virtual ~Entity() {}

	// add a behaviour
	void addBehaviour(Behaviour* behaviour);

	// update game object and execute behaviours
	virtual void executeBehaviours(float deltaTime);
	
	Blackboard&	getBlackboard() { return m_blackboard; }

protected:
	
	Blackboard				m_blackboard;
	std::vector<Behaviour*>	m_behaviours;
};

} // namespace ai