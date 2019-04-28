#pragma once

#include "Behaviour.h"

class FollowBehaviour : public Behaviour {
public:

	FollowBehaviour();
	virtual ~FollowBehaviour();

	virtual eBehaviourResult execute(Entity* entity, float deltaTime);

	void setSpeed(float speed) { m_speed = speed; }

	void setTarget(Entity* entity) { m_target = entity; }

private:

	float		m_speed;
	Entity*	m_target;
};