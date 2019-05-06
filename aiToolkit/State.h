#pragma once

#include "Behaviour.h"
#include "Condition.h"
#include <vector>

namespace ai {

class State;

class Transition {
public:

	Transition(State* target, Condition* condition) : m_target(target), m_condition(condition) {}
	~Transition() {}

	State* getTargetState() { return m_target; }

	bool hasTriggered(Entity* entity) { return m_condition->test(entity); }

private:

	State*		m_target;
	Condition*	m_condition;
};

// abstract class
class State {

	friend class FiniteStateMachine;

public:

	State() : m_timer(0) {}
	virtual ~State() {}

	// pure virtual
	virtual void	update(Entity* entity, float deltaTime) = 0;

	// triggers for enter / exit
	virtual void	onEnter(Entity* entity) {}
	virtual void	onExit(Entity* entity) {}

	void addTransition(Transition* transition) {
		m_transitions.push_back(transition);
	}

	Transition*	getTriggeredTransition(Entity* entity);

	const float* getTimerPtr() const { return &m_timer; }

	float getTimer() const { return m_timer; }

protected:

	float	m_timer;
	std::vector<Transition*>	m_transitions;
};

class FiniteStateMachine : public Behaviour {
public:

	FiniteStateMachine() {}

	virtual ~FiniteStateMachine() {
		for (auto state : m_states)
			delete state;
		for (auto t : m_transitions)
			delete t;
		for (auto c : m_conditions)
			delete c;
	}

	// add components, takes ownership
	State*		addState(State* state) { m_states.push_back(state); return state; }
	Transition*	addTransition(Transition* transition) {	m_transitions.push_back(transition); return transition;	}
	Condition*	addCondition(Condition* condition) { m_conditions.push_back(condition); return condition; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime);

protected:

	std::vector<State*>			m_states;
	std::vector<Transition*>	m_transitions;
	std::vector<Condition*>		m_conditions;
};

} // namespace ai