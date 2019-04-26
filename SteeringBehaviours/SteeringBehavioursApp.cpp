#include "SteeringBehavioursApp.h"
#include "Font.h"
#include "Input.h"

#include "aiUtilities.h"

SteeringBehavioursApp::SteeringBehavioursApp() {

}

SteeringBehavioursApp::~SteeringBehavioursApp() {

}

bool SteeringBehavioursApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);
		
	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);
	m_player.addBehaviour(&m_keyboardBehaviour);

	// setup steering properties for zombies
	m_seek.setTarget(&m_player);
	m_flee.setTarget(&m_player);
	m_avoid.setFeelerLength(80);

	// set up "zombie" finite state machine using steering
	m_attackState = new SteeringState();
	m_attackState->addForce(&m_wander, .2f);
	m_attackState->addForce(&m_seek, .8f);
	m_attackState->addForce(&m_flee, 0);
	m_attackState->addForce(&m_avoid, 1);

	SteeringState* wanderState = new SteeringState();
	wanderState->addForce(&m_wander, 1);
	wanderState->addForce(&m_avoid, 1);

	// set conditions for state transitions
	Condition* withinRangeCondition = new WithinRangeCondition(&m_player, 200);
	Condition* notWithinRangeCondition = new NotCondition(withinRangeCondition);

	Transition* withinRange = new Transition(m_attackState, withinRangeCondition);
	Transition* notWithinRange = new Transition(wanderState, notWithinRangeCondition);

	m_attackState->addTransition(notWithinRange);
	wanderState->addTransition(withinRange);

	m_fsm.addCondition(withinRangeCondition);
	m_fsm.addCondition(notWithinRangeCondition);

	m_fsm.addTransition(withinRange);
	m_fsm.addTransition(notWithinRange);

	m_fsm.addState(m_attackState);
	m_fsm.addState(wanderState);

	// setup enemies
	for (auto& enemy : m_enemies) {

		enemy.addBehaviour(&m_fsm);
		enemy.getBlackboard().set("currentState", wanderState);

		Vector2* v = new Vector2();
		v->x = 0;
		v->y = 0;
		enemy.getBlackboard().set("velocity", v, true);

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;
		enemy.getBlackboard().set("wanderData", wd, true);

		enemy.getBlackboard().set("maxForce", 300.f);
		enemy.getBlackboard().set("maxVelocity", 150.f);
		
		enemy.setPosition( float(rand() % getWindowWidth()),
						   float(rand() % getWindowHeight()));
	}

	// set up my obstacles
	for (int i = 0; i < 10; ++i) {

		Obstacle o;
		o.x = rand() % (getWindowWidth() - 150) + 75.f;
		o.y = rand() % (getWindowHeight() - 150) + 75.f;

		if (rand() % 20 < 10) {
			o.type = Obstacle::SPHERE;
			o.r = rand() % 40 + 40.f;
			m_avoid.addSphereObstacle(o.x, o.y, o.r);
		}
		else {
			o.type = Obstacle::BOX;
			o.w = o.h = rand() % 50 + 100.f;
			m_avoid.addBoxObstacle(o.x, o.y, o.w, o.h);
		}

		m_obstacles.push_back(o);
	}

	return true;
}

void SteeringBehavioursApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void SteeringBehavioursApp::update(float deltaTime) {
	
	// update behaviours
	m_player.update(deltaTime);

	for (auto& enemy : m_enemies)
		enemy.update(deltaTime);

	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();

	// change the zombies from angry to scared and vice versa
	if (input->wasKeyPressed(INPUT_KEY_F)) {
		m_attackState->setWeightForForce(&m_flee, 0.8f);
		m_attackState->setWeightForForce(&m_seek, 0);
	}
	if (input->wasKeyPressed(INPUT_KEY_S)) {
		m_attackState->setWeightForForce(&m_flee, 0);
		m_attackState->setWeightForForce(&m_seek, 0.8f);
	}
}

void SteeringBehavioursApp::screenWrap(float& x, float& y) {
	// wrap position around the screen
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();
	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}

void SteeringBehavioursApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw obstacles as pink circles
	for (auto obstacle : m_obstacles) {
		m_2dRenderer->setRenderColour(1, 0, 1);
		if (obstacle.type == Obstacle::SPHERE)
			m_2dRenderer->drawCircle(obstacle.x, obstacle.y, obstacle.r);
		if (obstacle.type == Obstacle::BOX)
			m_2dRenderer->drawBox(obstacle.x, obstacle.y, obstacle.w, obstacle.h);
	}

	float x = 0, y = 0;

	// draw player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	screenWrap(x, y);
	m_player.setPosition(x, y);

	float vx, vy;
	Vector2* v = nullptr;

	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	float s2 = sinf(3.14159f*-0.15f);
	float c2 = cosf(3.14159f*-0.15f);

	// draw enemies as a red circle
	for (auto& enemy : m_enemies) {
		enemy.getPosition(&x, &y);
		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawCircle(x, y, 10);

		screenWrap(x, y);
		enemy.setPosition(x, y);
		
		// draw feelers
		m_2dRenderer->setRenderColour(1, 1, 0);
		if (enemy.getBlackboard().get("velocity", &v)) {

			vx = v->x;
			vy = v->y;

			float magSqr = vx * vx + vy * vy;
			if (magSqr > 0) {
				magSqr = sqrt(magSqr);
				vx /= magSqr;
				vy /= magSqr;

				m_2dRenderer->drawLine(x, y, x + vx * 80, y + vy * 80);

				m_2dRenderer->drawLine(x, y, x + (vx * c - vy * s) * 80 * 0.5f, y + (vx * s + vy * c) * 80 * 0.5f);
				m_2dRenderer->drawLine(x, y, x + (vx * c2 - vy * s2) * 80 * 0.5f, y + (vx * s2 + vy * c2) * 80 * 0.5f);
			}
		}
	}

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "W for Wander, F for Flee, S for Seek", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}