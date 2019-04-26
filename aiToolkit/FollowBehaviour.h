#pragma once

#include "Behaviour.h"

class FollowBehaviour : public Behaviour {
public:

	FollowBehaviour();
	virtual ~FollowBehaviour();

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);

	void setSpeed(float speed) { m_speed = speed; }

	void setTarget(GameObject* gameObject) { m_target = gameObject; }

private:

	float		m_speed;
	GameObject*	m_target;
};