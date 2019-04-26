#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameObject.h"
#include "SteeringBehaviour.h"

enum eEntityClass : int {
	CIVILIAN = 0,
	MEDIC,
	FIREMAN,
};

enum eBlackboardQuestionType : int {
	REQUIRE_MEDIC,
	REQUIRE_FIREMAN,
};

// condition that checks blackboard
class BlackboardBoolCondition : public Condition {
public:

	BlackboardBoolCondition(const char* entry) : m_entry(entry) {}
	virtual ~BlackboardBoolCondition() {}

	virtual bool test(GameObject* gameObject) const {
		bool value = false;
		gameObject->getBlackboard().get(m_entry, value);
		return value;
	}

protected:

	std::string m_entry;
};

// condition that checks blackboard
class BlackboardHasEntryCondition : public Condition {
public:

	BlackboardHasEntryCondition(const char* entry) : m_entry(entry) {}
	virtual ~BlackboardHasEntryCondition() {}

	virtual bool test(GameObject* gameObject) const {
		return gameObject->getBlackboard().contains(m_entry);
	}

protected:

	std::string m_entry;
};

// a question that gets posted to the blackboard requesting help
// this could be done better
class NeedHelpQuestion : public BlackboardQuestion {
public:
	NeedHelpQuestion(int type) : BlackboardQuestion(type) {}
	virtual ~NeedHelpQuestion() {}

	GameObject* needsHelp;
};

// a game object that is also a blackboard expert
class MyGameObject : public GameObject, public BlackboardExpert {
public:

	MyGameObject() {}
	virtual ~MyGameObject() {}

	// blackboard methods
	virtual float	evaluateResponse(BlackboardQuestion* question, Blackboard* blackboard);
	virtual void	execute(BlackboardQuestion* question, Blackboard* blackboard);
};

// state does nothing
class IdleState : public State {
public:

	IdleState() {}
	virtual ~IdleState() {}
	virtual void	update(GameObject* gameObject, float deltaTime) {}
};

// game object moves towards its target
// when it gets there it marks blackboards as success
class HelpEntityState : public State {
public:

	HelpEntityState() {}
	virtual ~HelpEntityState() {}
	virtual void	update(GameObject* gameObject, float deltaTime);
};

// behaviour that responds to questions this entity can answer
class BlackboardRespondBehaviour : public Behaviour {
public:

	BlackboardRespondBehaviour(Blackboard* blackboard) : m_blackboard(blackboard) {}
	virtual ~BlackboardRespondBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);

protected:

	Blackboard*	m_blackboard;
};

// demo application
class BlackboardsApp : public Application {
public:

	BlackboardsApp();
	virtual ~BlackboardsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void screenWrap(float& x, float& y);

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	MyGameObject		m_entities[30];

	// shared state machine and wander force
	FiniteStateMachine	m_fsm;
	WanderForce			m_wander;

	// shared behaviour for responding to help requests
	BlackboardRespondBehaviour	m_respondBehaviour;

	// timer for tracking when to request help (a hack for now)
	// to do this properly the behaviour of the entity's
	// would request help when it is hurt
	float				m_someoneNeedsHelpTimer;

	// shared questions that get posted requesting help
	NeedHelpQuestion	m_requireFiremanQuestion;
	NeedHelpQuestion	m_requireMedicQuestion;

	// shared "level" blackboard for global state data
	Blackboard			m_globalBlackboard;
};