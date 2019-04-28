#include "State.h"

Transition* State::getTriggeredTransition(Entity* entity) {

	for (auto transition : m_transitions) {
		if (transition->hasTriggered(entity))
			return transition;
	}

	return nullptr;
}

eBehaviourResult FiniteStateMachine::execute(Entity* entity, float deltaTime) {

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
		state->m_timer += deltaTime;
		state->update(entity, deltaTime);

		return eBehaviourResult::SUCCESS;
	}

	return eBehaviourResult::FAILURE;
}