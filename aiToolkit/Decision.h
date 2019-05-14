#pragma once

#include "Behaviour.h"
#include "Condition.h"
#include "SteeringBehaviour.h"
#include "Timing.h"

namespace ai {

// base decision tree class
class Decision {
public:

	Decision() {}
	virtual ~Decision() {}
	
	virtual void makeDecision(Agent* entity) = 0;
};

// conditional decision
// has true and false branches and uses a Condition to decide
class ConditionalDecision : public Decision {
public:

	ConditionalDecision() :
		m_condition(nullptr), m_trueBranch(nullptr), m_falseBranch(nullptr) {
	}
	ConditionalDecision(Condition* condition, Decision* trueBranch, Decision* falseBranch) :
		m_condition(condition), m_trueBranch(trueBranch), m_falseBranch(falseBranch) {}

	virtual ~ConditionalDecision() {}

	void setCondition(Condition* condition) { m_condition = condition; }
	void setTrueBranch(Decision* decision) { m_trueBranch = decision; }
	void setFalseBranch(Decision* decision) { m_falseBranch = decision; }

	virtual void makeDecision(Agent* entity) {

		if (m_condition != nullptr &&
			m_trueBranch != nullptr &&
			m_falseBranch != nullptr) {

			if (m_condition->test(entity))
				m_trueBranch->makeDecision(entity);
			else
				m_falseBranch->makeDecision(entity);
		}
	}

protected:

	Condition*	m_condition;
	Decision*	m_trueBranch;
	Decision*	m_falseBranch;
};

// a behaviour that can be attached to a Entity that
// uses a Decision to perform actions (can be a tree or single decision)
class DecisionBehaviour : public Behaviour {
public:

	DecisionBehaviour(Decision* decision = nullptr) : m_decision(decision) {}
	virtual ~DecisionBehaviour() {}

	void setDecision(Decision* decision) { m_decision = decision; }

	virtual eBehaviourResult execute(Agent* entity) {

		if (m_decision != nullptr) {
			m_decision->makeDecision(entity);
			return eBehaviourResult::SUCCESS;
		}
		return eBehaviourResult::FAILURE;
	}

protected:

	Decision* m_decision;
};

// decision that executes a behaviour
// example usage could be a decision that holds a FSM
class BehaviourDecision : public Decision {
public:

	BehaviourDecision(Behaviour* behaviour = nullptr) : m_behaviour(behaviour) {}
	virtual ~BehaviourDecision() {}

	void setBehaviour(Behaviour* behaviour) { m_behaviour = behaviour; }

	virtual void makeDecision(Agent* entity) {

		if (m_behaviour != nullptr)
			m_behaviour->execute(entity);
	}

protected:

	Behaviour*	m_behaviour;
};

// a decision that executes a single steering force
class SteeringDecision : public Decision {
public:

	SteeringDecision(SteeringForce* force = nullptr) : m_force(force) {}
	virtual ~SteeringDecision() {}

	void setForce(SteeringForce* force) { m_force = force; }

	virtual void makeDecision(Agent* entity) {

		glm::vec3* velocity = nullptr;

		// must have velocity
		if (entity->getBlackboard().get("velocity", &velocity) == false)
			return;

		// apply force to velocity
		auto force = m_force->getForce(entity);

		*velocity += force * app::Time::deltaTime();

		float maxVelocity = 0;
		entity->getBlackboard().get("maxVelocity", maxVelocity);

		// cap velocity
		float magnitudeSqr = glm::dot(*velocity, *velocity);
		if (magnitudeSqr > (maxVelocity * maxVelocity))
			*velocity /= sqrt(magnitudeSqr) * maxVelocity;

		// move the game object
		entity->translate(*velocity * app::Time::deltaTime());
	}

protected:

	SteeringForce*	m_force;
};

// random sub-branch decision
class RandomDecision : public Decision {
public:

	RandomDecision() {}
	virtual ~RandomDecision() {}

	void addDecision(Decision* decision) { m_decisions.push_back(decision); }
	
	virtual void makeDecision(Agent* entity) {

		if (m_decisions.empty() == false) {
			m_decisions[rand() % m_decisions.size()]->makeDecision(entity);
		}
	}

protected:

	std::vector<Decision*>	m_decisions;

};

} // namespace ai