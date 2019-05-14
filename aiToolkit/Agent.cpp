#include "Agent.h"
#include "Behaviour.h"

namespace ai {

void Agent::addBehaviour(Behaviour* behaviour) {
	m_behaviours.push_back(behaviour);
}

void Agent::executeBehaviours() {
	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this);
}

} // namespace ai