#include "FiniteStateMachineApp.h"
#include "Font.h"
#include "Input.h"

#include "GuardStates.h"

FiniteStateMachineApp::FiniteStateMachineApp() {

}

FiniteStateMachineApp::~FiniteStateMachineApp() {

}

bool FiniteStateMachineApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);

	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.addBehaviour(&m_keyboardBehaviour);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);

	// guard
	m_enemy.addBehaviour(&m_guardFSM);

	// created new states
	auto attackState = new AttackState(&m_player, 150);
	auto idleState = new IdleState();
	auto patrolState = new PatrolState(75);

	// setup our patrol path
	patrolState->addWaypoint(getWindowWidth() * 0.15f, getWindowHeight() * 0.15f);
	patrolState->addWaypoint(getWindowWidth() * 0.15f, getWindowHeight() * 0.85f);
	patrolState->addWaypoint(getWindowWidth() * 0.85f, getWindowHeight() * 0.85f);
	patrolState->addWaypoint(getWindowWidth() * 0.85f, getWindowHeight() * 0.15f);

	// setup conditions that will trigger transitions
	auto idleTimerCondition = new FloatGreaterCondition(idleState->getTimerPtr(), 2);
	auto withinRangeCondition = new WithinRangeCondition(&m_player, 200);
	auto outsideRangeCondition = new NotCondition(withinRangeCondition);

	// add transitions
	auto attackToIdleTransition = new Transition(idleState, outsideRangeCondition);
	auto toAttackTransition = new Transition(attackState, withinRangeCondition);
	auto idleToPatrol = new Transition(patrolState, idleTimerCondition);

	// attack to idle
	attackState->addTransition(attackToIdleTransition);

	// idle to attack
	idleState->addTransition(idleToPatrol);
	idleState->addTransition(toAttackTransition);

	// patrol to attack
	patrolState->addTransition(toAttackTransition);

	// set our starting state to patrol
	m_enemy.getBlackboard().set("currentState", patrolState);

	// store everything in state machine (just for memory cleanup)
	m_guardFSM.addState(attackState);
	m_guardFSM.addState(idleState);
	m_guardFSM.addState(patrolState);

	m_guardFSM.addCondition(withinRangeCondition);
	m_guardFSM.addCondition(idleTimerCondition);
	m_guardFSM.addCondition(outsideRangeCondition);

	m_guardFSM.addTransition(attackToIdleTransition);
	m_guardFSM.addTransition(toAttackTransition);
	m_guardFSM.addTransition(idleToPatrol);

	return true;
}

void FiniteStateMachineApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void FiniteStateMachineApp::update(float deltaTime) {

	m_player.executeBehaviours(deltaTime);
	m_enemy.executeBehaviours(deltaTime);

	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();
}

void FiniteStateMachineApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x = 0, y = 0;

	// draw player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	// draw enemy as a red circle
	m_enemy.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(1, 0, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	// draw enemy range
	m_2dRenderer->setRenderColour(1, 1, 0, 0.25f);
	m_2dRenderer->drawCircle(x, y, 200);

	// output some text
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0, -0.1f);

	// done drawing sprites
	m_2dRenderer->end();
}