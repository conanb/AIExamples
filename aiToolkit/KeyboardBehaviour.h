#pragma once

#include "Behaviour.h"

namespace ai {

class KeyboardBehaviour : public Behaviour {
public:

	KeyboardBehaviour();
	virtual ~KeyboardBehaviour();

	virtual eBehaviourResult execute(Entity* entity);

	void setSpeed(float speed) { m_speed = speed; }

private:

	float m_speed;
};

} // namespace ai