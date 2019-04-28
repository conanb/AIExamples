#include "FollowBehaviour.h"

FollowBehaviour::FollowBehaviour()
	: m_speed(1),
	m_target(nullptr) {
}

FollowBehaviour::~FollowBehaviour() {

}

eBehaviourResult FollowBehaviour::execute(Entity* entity, float deltaTime) {

	if (m_target == nullptr)
		return eBehaviourResult::FAILURE;

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

	return eBehaviourResult::SUCCESS;
}