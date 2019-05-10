#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
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
class BlackboardBoolCondition : public ai::Condition {
public:

	BlackboardBoolCondition(const char* entry) : m_entry(entry) {}
	virtual ~BlackboardBoolCondition() {}

	virtual bool test(ai::Entity* entity) const {
		bool value = false;
		entity->getBlackboard().get(m_entry, value);
		return value;
	}

protected:

	std::string m_entry;
};

// condition that checks blackboard
class BlackboardHasEntryCondition : public ai::Condition {
public:

	BlackboardHasEntryCondition(const char* entry) : m_entry(entry) {}
	virtual ~BlackboardHasEntryCondition() {}

	virtual bool test(ai::Entity* entity) const {
		return entity->getBlackboard().contains(m_entry);
	}

protected:

	std::string m_entry;
};

// a question that gets posted to the blackboard requesting help
// this could be done better
class NeedHelpQuestion : public ai::BlackboardQuestion {
public:
	NeedHelpQuestion(int type) : BlackboardQuestion(type) {}
	virtual ~NeedHelpQuestion() {}

	ai::Entity* needsHelp;
};

// a game object that is also a blackboard expert
class MyEntity : public ai::Entity, public ai::BlackboardExpert {
public:

	MyEntity() {}
	virtual ~MyEntity() {}

	// blackboard methods
	virtual float	evaluateResponse(ai::BlackboardQuestion* question, ai::Blackboard* blackboard);
	virtual void	execute(ai::BlackboardQuestion* question, ai::Blackboard* blackboard);
};

// state does nothing
class IdleState : public ai::State {
public:

	IdleState() {}
	virtual ~IdleState() {}
	virtual void	update(ai::Entity* entity) {}
};

// game object moves towards its target
// when it gets there it marks blackboards as success
class HelpEntityState : public ai::State {
public:

	HelpEntityState() {}
	virtual ~HelpEntityState() {}
	virtual void	update(ai::Entity* entity);
};

// behaviour that responds to questions this entity can answer
class BlackboardRespondBehaviour : public ai::Behaviour {
public:

	BlackboardRespondBehaviour(ai::Blackboard* blackboard) : m_blackboard(blackboard) {}
	virtual ~BlackboardRespondBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity);

protected:

	ai::Blackboard*	m_blackboard;
};

// demo application
class BlackboardsApp : public app::Application {
public:

	BlackboardsApp();
	virtual ~BlackboardsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:
	
	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	MyEntity		m_entities[30];

	// shared state machine and wander force
	ai::FiniteStateMachine	m_fsm;
	ai::WanderForce			m_wander;

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
	ai::Blackboard			m_globalBlackboard;
};