#include "BehaviourTree.h"

#include <GLFW/glfw3.h>

eBehaviourResult TimeoutDecorator::execute(Entity* entity, float deltaTime) {

	float currTime = (float)glfwGetTime();

	if (m_child != nullptr &&
		(m_lastTime == -1 ||
		 (currTime - m_lastTime) >= m_cooldown)) {

		m_lastTime = currTime;

		return m_child->execute(entity, deltaTime);
	}

	return eBehaviourResult::FAILURE;
}