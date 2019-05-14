#include "State.h"
#include "Timing.h"

namespace ai {

Transition* State::getTriggeredTransition(Agent* entity) {

	for (auto transition : m_transitions) {
		if (transition->hasTriggered(entity))
			return transition;
	}

	return nullptr;
}

eBehaviourResult FiniteStateMachine::execute(Agent* entity) {

	State* state = nullptr;
	entity->getBlackboard().get("currentState", &state);
	if (state != nullptr) {

		Transition* transition = state->getTriggeredTransition(entity);

		if (transition != nullptr) {

			state->onExit(entity);

			state = transition->getTargetState();
			entity->getBlackboard().set("currentState", state);

			state->m_timer = 0;
			state->onEnter(entity);
		}

		// accumulate time and update state
		state->m_timer += app::Time::deltaTime();
		state->update(entity);

		return eBehaviourResult::SUCCESS;
	}

	return eBehaviourResult::FAILURE;
}

} // namespace ai