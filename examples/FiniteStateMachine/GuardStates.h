#pragma once

#include "State.h"

// moves towards target at a speed
class AttackState : public ai::State {
public:

	AttackState(ai::Agent* target, float speed) : m_target(target), m_speed(speed) {}
	virtual ~AttackState() {}

	virtual void	update(ai::Agent* entity);

private:

	float		m_speed;
	ai::Agent*	m_target;
};

// just does nothing
class IdleState : public ai::State {
public:

	IdleState() {}
	virtual ~IdleState() {}

	virtual void	update(ai::Agent* entity) {}
};

// paths between a sequence of points and loops
class PatrolState : public ai::State {
public:

	PatrolState(float speed) : m_currentTarget(0), m_speed(speed) {}
	virtual ~PatrolState() {}

	void addWaypoint(float x, float y) {
		m_locations.push_back({ x,y });
	}

	virtual void	update(ai::Agent* entity);

protected:

	float					m_speed;
	unsigned int			m_currentTarget;
	std::vector<glm::vec2>	m_locations;
};