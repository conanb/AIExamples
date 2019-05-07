#include "FollowBehaviour.h"
#include "Timing.h"

namespace ai {

FollowBehaviour::FollowBehaviour()
	: m_speed(1),
	m_target(nullptr) {
}

FollowBehaviour::~FollowBehaviour() {

}

eBehaviourResult FollowBehaviour::execute(Entity* entity) {

	if (m_target == nullptr)
		return eBehaviourResult::FAILURE;

	// get target position
	auto target = m_target->getPosition();
	
	// get my position
	auto position = entity->getPosition();

	// compare the two and get the difference between them
	auto diff = target - position;

	// if not at the target then move towards them
	if (glm::length(diff) > 0) {

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		diff = glm::normalize(diff);

		// move to target (can overshoot!)
		entity->translate(diff * m_speed * app::Time::deltaTime());
	}

	return eBehaviourResult::SUCCESS;
}

} // namespace ai