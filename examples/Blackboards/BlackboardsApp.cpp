#include "BlackboardsApp.h"
#include "Font.h"
#include "Input.h"

BlackboardsApp::BlackboardsApp()
	: m_requireFiremanQuestion(eBlackboardQuestionType::REQUIRE_FIREMAN),
	m_requireMedicQuestion(eBlackboardQuestionType::REQUIRE_MEDIC),
	m_respondBehaviour(&m_globalBlackboard) {

}

BlackboardsApp::~BlackboardsApp() {

}

bool BlackboardsApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);

	// setup the behaviour of entities

	auto wanderState = new SteeringState();
	wanderState->addForce(&m_wander);

	auto idleState = new IdleState();
	auto helpState = new HelpEntityState();

	// conditions for triggering transitions
	auto needHelpCondition = new BlackboardBoolCondition("requireHelp");
	auto helpingCondition = new BlackboardHasEntryCondition("target");
	auto dontNeedHelpCondition = new NotCondition(needHelpCondition);
	auto notHelpingCondition = new NotCondition(helpingCondition);

	// transitions between states
	auto toHelpingTransition = new Transition(helpState, helpingCondition);
	auto toIdleTransition = new Transition(idleState, needHelpCondition);
	auto toIdleFromHelpingTransition = new Transition(idleState, notHelpingCondition);
	auto toWanderTransition = new Transition(wanderState, dontNeedHelpCondition);

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

		Vector2* v = new Vector2();
		v->x = 0;
		v->y = 0;
		blackboard.set("velocity", v, true);

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;
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

		go.setPosition(float(rand() % getWindowWidth()),
					   float(rand() % getWindowHeight()));
	}

	return true;
}

void BlackboardsApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BlackboardsApp::update(float deltaTime) {

	// post question every few seconds
	// this is a hack: usually an entity's behaviour would
	// make it ask for help
	m_someoneNeedsHelpTimer -= deltaTime;
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
		entity.executeBehaviours(deltaTime);

	// arbitrate questions
	m_globalBlackboard.runArbitration();

	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();
}

void BlackboardsApp::screenWrap(float& x, float& y) {
	// wrap position around the screen
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();
	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}

void BlackboardsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	int entityClass = eEntityClass::CIVILIAN;
	float x, y;

	// draw entities
	for (auto& go : m_entities) {

		go.getBlackboard().get("class", entityClass);

		// different colour based on class
		switch (entityClass) {
		case eEntityClass::MEDIC:	m_2dRenderer->setRenderColour(1, 0, 1);	break;
		case eEntityClass::FIREMAN:	m_2dRenderer->setRenderColour(1, 1, 0);	break;
		default:	m_2dRenderer->setRenderColour(0, 1, 1);	break;
		};

		go.getPosition(&x, &y);
		screenWrap(x, y);
		go.setPosition(x, y);

		m_2dRenderer->drawCircle(x, y, 10);

		// draw red circle around entities needing help
		bool requireHelp = false;
		go.getBlackboard().get("requireHelp", requireHelp);
		if (requireHelp) {
			m_2dRenderer->setRenderColour(1, 0, 0);
			m_2dRenderer->drawCircle(x, y, 20);
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

float Myentity::evaluateResponse(BlackboardQuestion* question, Blackboard* blackboard) {

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

		float tx, ty;
		q->needsHelp->getPosition(&tx, &ty);

		float xDiff = tx - m_x;
		float yDiff = ty - m_y;

		return 9999999.0f - (xDiff * xDiff + yDiff * yDiff);
	}
	return 0;
}

void Myentity::execute(BlackboardQuestion* question, Blackboard* blackboard) {

	NeedHelpQuestion* q = (NeedHelpQuestion*)question;

	// changes state to helping questioner
	m_blackboard.set("target", q->needsHelp);
}

void HelpEntityState::update(Entity* entity, float deltaTime) {

	Entity* target = nullptr;
	entity->getBlackboard().get("target", &target);

	if (target == nullptr)
		return;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	// move to target
	// get target position
	float tx = 0, ty = 0;
	target->getPosition(&tx, &ty);

	// get my position
	float x = 0, y = 0;
	entity->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = tx - x;
	float yDiff = ty - y;
	float distance = xDiff * xDiff + yDiff * yDiff;

	// if not at the target then move towards them
	if (distance > 25) {

		distance = sqrt(distance);
		xDiff /= distance;
		yDiff /= distance;

		// move to target
		entity->translate(xDiff * speed * deltaTime, yDiff * speed * deltaTime);
	}
	else {
		// tag, they're helped!
		target->getBlackboard().set("requireHelp", false);
		entity->getBlackboard().remove("target");
	}
}

// go through questions and respond to those we think are valid
eBehaviourResult BlackboardRespondBehaviour::execute(Entity* entity, float deltaTime) {

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
			question->addExpert((Myentity*)entity);
	}

	return eBehaviourResult::SUCCESS;
}