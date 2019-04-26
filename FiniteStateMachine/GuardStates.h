#pragma once

#include "State.h"

// moves towards target at a speed
class AttackState : public State {
public:

	AttackState(GameObject* target, float speed) : m_target(target), m_speed(speed) {}
	virtual ~AttackState() {}

	virtual void	update(GameObject* gameObject, float deltaTime);

private:

	float		m_speed;
	GameObject*	m_target;
};

// just does nothing
class IdleState : public State {
public:

	IdleState() {}
	virtual ~IdleState() {}

	virtual void	update(GameObject* gameObject, float deltaTime) {}
};

// paths between a sequence of points and loops
class PatrolState : public State {
public:

	PatrolState(float speed) : m_currentTarget(0), m_speed(speed) {}
	virtual ~PatrolState() {}

	void addWaypoint(float x, float y) {
		Location loc = { x, y };
		m_locations.push_back(loc);
	}

	virtual void	update(GameObject* gameObject, float deltaTime);

protected:

	struct Location {
		float x, y;
	};

	float					m_speed;
	unsigned int			m_currentTarget;
	std::vector<Location>	m_locations;
};