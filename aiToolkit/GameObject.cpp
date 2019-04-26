#include "GameObject.h"
#include "Behaviour.h"

GameObject::GameObject() 
	: m_x(0),
	m_y(0) {

}

GameObject::~GameObject() {

}

void GameObject::addBehaviour(Behaviour* behaviour) {

	m_behaviours.push_back(behaviour);
}

void GameObject::update(float deltaTime) {

	// execute all behaviours
	for (auto behaviour : m_behaviours)
		behaviour->execute(this, deltaTime);
}