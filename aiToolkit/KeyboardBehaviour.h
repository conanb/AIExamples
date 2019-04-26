#pragma once

#include "Behaviour.h"

class KeyboardBehaviour : public Behaviour {
public:

	KeyboardBehaviour();
	virtual ~KeyboardBehaviour();

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);

	void setSpeed(float speed) { m_speed = speed; }

private:

	float m_speed;
};