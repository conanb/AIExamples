#include "GuardStates.h"
#include "Timing.h"

void AttackState::update(ai::Agent* entity) {

	if (m_target == nullptr)
		return;

	// get target position
	auto target = m_target->getPosition();

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = target - position;

	// if not at the target then move towards them
	if (glm::dot(diff,diff) > 0) {
		
		// move to target (can overshoot!)
		entity->translate(glm::normalize(diff) * m_speed * app::Time::deltaTime());
	}
}

void PatrolState::update(ai::Agent* entity) {

	if (m_locations.empty())
		return;

	auto target = m_locations[m_currentTarget];

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = glm::vec3(target,0) - position;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 10) {

		// move to target (can overshoot!)
		entity->translate(glm::normalize(diff) * m_speed * app::Time::deltaTime());
	}
	else {
		// go to next target!
		if (++m_currentTarget >= m_locations.size())
			m_currentTarget = 0;
	}
}