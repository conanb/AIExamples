#pragma once

#include "Behaviour.h"

namespace ai {

class FollowBehaviour : public Behaviour {
public:

	FollowBehaviour();
	virtual ~FollowBehaviour();

	virtual eBehaviourResult execute(Agent* entity);

	void setSpeed(float speed) { m_speed = speed; }

	void setTarget(Agent* entity) { m_target = entity; }

private:

	float		m_speed;
	Agent*	m_target;
};

} // namespace ai