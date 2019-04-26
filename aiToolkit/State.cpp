#include "State.h"

Transition* State::getTriggeredTransition(GameObject* gameObject) {

	for (auto transition : m_transitions) {
		if (transition->hasTriggered(gameObject))
			return transition;
	}

	return nullptr;
}

eBehaviourResult FiniteStateMachine::execute(GameObject* gameObject, float deltaTime) {

	State* state = nullptr;
	gameObject->getBlackboard().get("currentState", &state);
	if (state != nullptr) {

		Transition* transition = state->getTriggeredTransition(gameObject);

		if (transition != nullptr) {

			state->onExit(gameObject);

			state = transition->getTargetState();
			gameObject->getBlackboard().set("currentState", state);

			state->m_timer = 0;
			state->onEnter(gameObject);
		}

		// accumulate time and update state
		state->m_timer += deltaTime;
		state->update(gameObject, deltaTime);

		return eBehaviourResult::SUCCESS;
	}

	return eBehaviourResult::FAILURE;
}