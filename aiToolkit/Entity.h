#pragma once

#include <vector>

#include "Blackboard.h"

class Behaviour;

struct Vector2 {
	float x, y;
};

class Entity {
public:

	Entity();
	virtual ~Entity();

	// movement functions
	void setPosition(float x, float y)			{ m_x = x; m_y = y; }
	void getPosition(float* x, float* y) const	{ *x = m_x; *y = m_y; }
	void translate(float x, float y)			{ m_x += x; m_y += y; }

	// add a behaviour
	void addBehaviour(Behaviour* behaviour);

	// update game object and execute behaviours
	virtual void executeBehaviours(float deltaTime);
	
	Blackboard&	getBlackboard() { return m_blackboard; }

protected:
	
	float m_x, m_y;
	
	Blackboard				m_blackboard;
	std::vector<Behaviour*>	m_behaviours;
};