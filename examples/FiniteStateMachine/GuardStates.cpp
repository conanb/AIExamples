#include "GuardStates.h"

void AttackState::update(Entity* entity, float deltaTime) {

	if (m_target == nullptr)
		return;

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get my position
	float x = 0, y = 0;
	entity->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = tx - x;
	float yDiff = ty - y;
	float distance = sqrtf(xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;

		// move to target (can overshoot!)
		entity->translate(xDiff * m_speed * deltaTime, yDiff * m_speed * deltaTime);
	}
}

void PatrolState::update(Entity* entity, float deltaTime) {

	if (m_locations.empty())
		return;

	Location loc = m_locations[m_currentTarget];

	// get my position
	float x = 0, y = 0;
	entity->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = loc.x - x;
	float yDiff = loc.y - y;
	float distance = sqrtf(xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 10) {

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;

		// move to target (can overshoot!)
		entity->translate(xDiff * m_speed * deltaTime, yDiff * m_speed * deltaTime);
	}
	else {
		// go to next target!
		if (++m_currentTarget >= m_locations.size())
			m_currentTarget = 0;
	}
}