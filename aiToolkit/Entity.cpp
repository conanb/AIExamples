#include "Entity.h"
#include "Behaviour.h"

namespace ai {

void Entity::addBehaviour(Behaviour* behaviour) {
	m_behaviours.push_back(behaviour);
}

void Entity::executeBehaviours() {
	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this);
}

} // namespace ai