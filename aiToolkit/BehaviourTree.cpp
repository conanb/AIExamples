#include "BehaviourTree.h"
#include "Timing.h"
#include <GLFW/glfw3.h>

namespace ai {

eBehaviourResult TimeoutDecorator::execute(Entity* entity) {

	float currTime = app::Time::now();

	if (m_child != nullptr &&
		(m_lastTime == -1 ||
		 (currTime - m_lastTime) >= m_cooldown)) {

		m_lastTime = currTime;

		return m_child->execute(entity);
	}

	return eBehaviourResult::FAILURE;
}

} // namespace ai