#pragma once

#include <vector>

#include "Blackboard.h"

namespace ai {

class Behaviour;

class Agent {
public:

	Agent() : m_transform(1) {}
	virtual ~Agent() {}

	// add a behaviour
	void addBehaviour(Behaviour* behaviour);

	// update game object and execute behaviours
	virtual void executeBehaviours();
	
	Blackboard&	getBlackboard() { return m_blackboard; }

	glm::vec3 getPosition() const { return m_transform[3]; }
	virtual void setPosition(const glm::vec3& v) { m_transform[3] = { v, 1 }; }
	virtual void translate(const glm::vec3& v) { m_transform[3] += glm::vec4(v, 0); }

	const glm::mat4& getTransform() const { return m_transform; }

protected:

	glm::mat4				m_transform;
	
	Blackboard				m_blackboard;
	std::vector<Behaviour*>	m_behaviours;
};

} // namespace ai