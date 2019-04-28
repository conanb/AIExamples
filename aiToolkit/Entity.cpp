#include "Entity.h"
#include "Behaviour.h"

Entity::Entity() 
	: m_x(0),
	m_y(0) {
}

Entity::~Entity() {
}

void Entity::addBehaviour(Behaviour* behaviour) {
	m_behaviours.push_back(behaviour);
}

void Entity::executeBehaviours(float deltaTime) {
	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this, deltaTime);
}