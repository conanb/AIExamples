#include "BlackboardsApp.h"
#include "Font.h"
#include "Input.h"
#include "Timing.h"

BlackboardsApp::BlackboardsApp()
	: m_requireFiremanQuestion(eBlackboardQuestionType::REQUIRE_FIREMAN),
	m_requireMedicQuestion(eBlackboardQuestionType::REQUIRE_MEDIC),
	m_respondBehaviour(&m_globalBlackboard) {

}

BlackboardsApp::~BlackboardsApp() {

}

bool BlackboardsApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	// setup the behaviour of entities

	auto wanderState = new ai::SteeringState();
	wanderState->addForce(&m_wander);

	auto idleState = new IdleState();
	auto helpState = new HelpEntityState();

	// conditions for triggering transitions
	auto needHelpCondition = new BlackboardBoolCondition("requireHelp");
	auto helpingCondition = new BlackboardHasEntryCondition("target");
	auto dontNeedHelpCondition = new ai::NotCondition(needHelpCondition);
	auto notHelpingCondition = new ai::NotCondition(helpingCondition);

	// transitions between states
	auto toHelpingTransition = new ai::Transition(helpState, helpingCondition);
	auto toIdleTransition = new ai::Transition(idleState, needHelpCondition);
	auto toIdleFromHelpingTransition = new ai::Transition(idleState, notHelpingCondition);
	auto toWanderTransition = new ai::Transition(wanderState, dontNeedHelpCondition);

	wanderState->addTransition(toIdleTransition);
	wanderState->addTransition(toHelpingTransition);
	idleState->addTransition(toWanderTransition);
	helpState->addTransition(toIdleFromHelpingTransition);

	// store in FSM for memory cleanup
	m_fsm.addState(idleState);
	m_fsm.addState(helpState);
	m_fsm.addState(wanderState);

	m_fsm.addTransition(toIdleTransition);
	m_fsm.addTransition(toIdleFromHelpingTransition);
	m_fsm.addTransition(toHelpingTransition);
	m_fsm.addTransition(toWanderTransition);

	m_fsm.addCondition(helpingCondition);
	m_fsm.addCondition(needHelpCondition);
	m_fsm.addCondition(dontNeedHelpCondition);
	m_fsm.addCondition(notHelpingCondition);

	m_someoneNeedsHelpTimer = 3;

	// setup entities
	for (auto& go : m_entities) {

		auto& blackboard = go.getBlackboard();

		glm::vec3* v = new glm::vec3();
		v->x = 0;
		v->y = 0;
		blackboard.set("velocity", v, true);

		ai::WanderData* wd = new ai::WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->target = glm::vec3(0);
		wd->axisWeights = glm::vec3(1, 1, 0);
		blackboard.set("wanderData", wd, true);

		blackboard.set("speed", 75.f);
		blackboard.set("maxForce", 300.f);
		blackboard.set("maxVelocity", 75.f);

		blackboard.set("currentState", wanderState);

		// store the class the entity belongs to
		// fireman / medic / civilian
		blackboard.set("class", rand() % 3);
		blackboard.set("requireHelp", false);

		go.addBehaviour(&m_fsm);
		go.addBehaviour(&m_respondBehaviour);

		go.setPosition({ float(rand() % getWindowWidth()),
					   float(rand() % getWindowHeight()), 0 });
	}

	return true;
}

void BlackboardsApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BlackboardsApp::update() {

	// post question every few seconds
	// this is a hack: usually an entity's behaviour would
	// make it ask for help
	m_someoneNeedsHelpTimer -= app::Time::deltaTime();
	if (m_someoneNeedsHelpTimer <= 0) {

		// post a question and pick a random that needs help
		auto go = &m_entities[rand() % 30];
		go->getBlackboard().set("requireHelp", true);

		if (rand() % 2 == 0) {
			m_requireFiremanQuestion.needsHelp = go;
			m_globalBlackboard.addQuestion(&m_requireFiremanQuestion);
		}
		else {
			m_requireMedicQuestion.needsHelp = go;
			m_globalBlackboard.addQuestion(&m_requireMedicQuestion);
		}

		// reset timer
		m_someoneNeedsHelpTimer = 3;
	}
	
	// update behaviours (will also respond to questions)
	for (auto& entity : m_entities)
		entity.executeBehaviours();

	// arbitrate questions
	m_globalBlackboard.runArbitration();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void BlackboardsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	int entityClass = eEntityClass::CIVILIAN;

	// draw entities
	for (auto& go : m_entities) {

		go.getBlackboard().get("class", entityClass);

		// different colour based on class
		switch (entityClass) {
		case eEntityClass::MEDIC:	m_2dRenderer->setRenderColour(1, 0, 1);	break;
		case eEntityClass::FIREMAN:	m_2dRenderer->setRenderColour(1, 1, 0);	break;
		default:	m_2dRenderer->setRenderColour(0, 1, 1);	break;
		};

		auto position = go.getPosition();
		screenWrap(position);
		go.setPosition(position);

		m_2dRenderer->drawCircle(position.x, position.y, 10);

		// draw red circle around entities needing help
		bool requireHelp = false;
		go.getBlackboard().get("requireHelp", requireHelp);
		if (requireHelp) {
			m_2dRenderer->setRenderColour(1, 0, 0);
			m_2dRenderer->drawCircle(position.x, position.y, 20);
		}
	}

	// output some text
	m_2dRenderer->setRenderColour(1, 0, 1);
	m_2dRenderer->drawText(m_font, "Medics", 0, 64);
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "Firemen", 0, 32);
	m_2dRenderer->setRenderColour(0, 1, 1);
	m_2dRenderer->drawText(m_font, "Civilians", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

float MyEntity::evaluateResponse(ai::BlackboardQuestion* question, ai::Blackboard* blackboard) {

	NeedHelpQuestion* q = (NeedHelpQuestion*)question;

	bool requireHelp = false;
	m_blackboard.get("requireHelp", requireHelp);

	// can't answer own request for help, or help if already helping
	// or help if we need help
	// response is BIGNUMBER - distance to entity requiring help
	if (q->needsHelp != nullptr &&
		q->needsHelp != this &&
		m_blackboard.contains("target") == false &&
		requireHelp == false) {

		auto target = q->needsHelp->getPosition();
		auto position = getPosition();

		auto diff = target - position;

		return 9999999.0f - glm::dot(diff, diff);
	}
	return 0;
}

void MyEntity::execute(ai::BlackboardQuestion* question, ai::Blackboard* blackboard) {

	NeedHelpQuestion* q = (NeedHelpQuestion*)question;

	// changes state to helping questioner
	m_blackboard.set("target", q->needsHelp);
}

void HelpEntityState::update(ai::Agent* entity) {

	ai::Agent* target = nullptr;
	entity->getBlackboard().get("target", &target);

	if (target == nullptr)
		return;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	// move to target
	// get target position
	auto targetPos = target->getPosition();

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = targetPos - position;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 25) {

		// move to target
		entity->translate(glm::normalize(diff) * speed * app::Time::deltaTime());
	}
	else {
		// tag, they're helped!
		target->getBlackboard().set("requireHelp", false);
		entity->getBlackboard().remove("target");
	}
}

// go through questions and respond to those we think are valid
ai::eBehaviourResult BlackboardRespondBehaviour::execute(ai::Agent* entity) {

	auto& questions = m_blackboard->getQuestions();

	int entityClass = eEntityClass::CIVILIAN;
	entity->getBlackboard().get("class", entityClass);

	for (auto& question : questions) {
		
		// can we respond?
		int type = question->getType();

		if ((type == eBlackboardQuestionType::REQUIRE_MEDIC &&
			 entityClass == eEntityClass::MEDIC) ||
			(type == eBlackboardQuestionType::REQUIRE_FIREMAN &&
			 entityClass == eEntityClass::FIREMAN))
			question->addExpert((MyEntity*)entity);
	}

	return ai::eBehaviourResult::SUCCESS;
}