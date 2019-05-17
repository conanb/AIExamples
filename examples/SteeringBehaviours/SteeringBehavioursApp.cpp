#include "SteeringBehavioursApp.h"
#include "Font.h"
#include "Input.h"

#include "Intersection.h"

SteeringBehavioursApp::SteeringBehavioursApp() {

}

SteeringBehavioursApp::~SteeringBehavioursApp() {

}

bool SteeringBehavioursApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);
		
	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.setPosition({ getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0 });
	m_player.addBehaviour(&m_keyboardBehaviour);

	// setup steering properties for zombies
	m_seek.setTarget(&m_player);
	m_flee.setTarget(&m_player);
	m_avoid.setFeelerLength(80);

	// set up "zombie" finite state machine using steering
	m_attackState = new ai::SteeringState();
	m_attackState->addForce(&m_wander, .2f);
	m_attackState->addForce(&m_seek, .8f);
	m_attackState->addForce(&m_flee, 0);
	m_attackState->addForce(&m_avoid, 1);

	ai::SteeringState* wanderState = new ai::SteeringState();
	wanderState->addForce(&m_wander, 1);
	wanderState->addForce(&m_avoid, 1);

	// set conditions for state transitions
	ai::Condition* withinRangeCondition = new ai::WithinRangeCondition(&m_player, 200);
	ai::Condition* notWithinRangeCondition = new ai::NotCondition(withinRangeCondition);

	ai::Transition* withinRange = new ai::Transition(m_attackState, withinRangeCondition);
	ai::Transition* notWithinRange = new ai::Transition(wanderState, notWithinRangeCondition);

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

		glm::vec3* v = new glm::vec3(0,0,0);
		enemy.getBlackboard().set("velocity", v, true);

		ai::WanderData* wd = new ai::WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->target = glm::vec3(0);
		wd->axisWeights = glm::vec3(1, 1, 0);
		enemy.getBlackboard().set("wanderData", wd, true);

		enemy.getBlackboard().set("maxForce", 300.f);
		enemy.getBlackboard().set("maxVelocity", 150.f);
		
		enemy.setPosition({ float(rand() % getWindowWidth()),
						   float(rand() % getWindowHeight()), 0 });
	}

	// set up my obstacles
	for (int i = 0; i < 8; ++i) {

		ai::Obstacle o;
		o.center.x = rand() % (getWindowWidth() - 150) + 75.f;
		o.center.y = rand() % (getWindowHeight() - 150) + 75.f;
		o.center.z = 0;

		/*if (rand() % 20 < 10) {
			o.type = ai::Obstacle::SPHERE;
			o.radius = rand() % 40 + 40.f;
			m_avoid.addSphereObstacle(o.center.x, o.center.y, o.center.z, o.radius);
		}
		else*/ {
			o.type = ai::Obstacle::BOX;
			o.extents.x = o.extents.y = rand() % 50 + 100.f;
			m_avoid.addBoxObstacle(o.center.x, o.center.y, o.center.z, o.extents.x, o.extents.y);
		}

		m_obstacles.push_back(o);
	}

	return true;
}

void SteeringBehavioursApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void SteeringBehavioursApp::update() {
	
	// update behaviours
	m_player.executeBehaviours();

	for (auto& enemy : m_enemies)
		enemy.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	// change the zombies from angry to scared and vice versa
	if (input->wasKeyPressed(app::INPUT_KEY_F)) {
		m_attackState->setWeightForForce(&m_flee, 0.8f);
		m_attackState->setWeightForForce(&m_seek, 0);
	}
	if (input->wasKeyPressed(app::INPUT_KEY_S)) {
		m_attackState->setWeightForForce(&m_flee, 0);
		m_attackState->setWeightForForce(&m_seek, 0.8f);
	}
}

void SteeringBehavioursApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw obstacles as pink circles
	for (auto obstacle : m_obstacles) {
		m_2dRenderer->setRenderColour(1, 0, 1);
		if (obstacle.type == ai::Obstacle::SPHERE)
			m_2dRenderer->drawCircle(obstacle.center.x, obstacle.center.y, obstacle.radius);
		if (obstacle.type == ai::Obstacle::BOX)
			m_2dRenderer->drawBox(obstacle.center.x, obstacle.center.y, obstacle.extents.x, obstacle.extents.y);
	}
	
	// draw player as a green circle
	auto position = m_player.getPosition();
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	screenWrap(position);
	m_player.setPosition(position);

	float vx, vy;
	glm::vec3* v = nullptr;

	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	float s2 = sinf(3.14159f*-0.15f);
	float c2 = cosf(3.14159f*-0.15f);

	// draw enemies as a red circle
	for (auto& enemy : m_enemies) {
		position = enemy.getPosition();
		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawCircle(position.x, position.y, 10);

		screenWrap(position);
		enemy.setPosition(position);
		
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

				m_2dRenderer->drawLine(position.x, position.y, position.x + vx * 80, position.y + vy * 80);

				m_2dRenderer->drawLine(position.x, position.y, position.x + (vx * c - vy * s) * 80 * 0.5f, position.y + (vx * s + vy * c) * 80 * 0.5f);
				m_2dRenderer->drawLine(position.x, position.y, position.x + (vx * c2 - vy * s2) * 80 * 0.5f, position.y + (vx * s2 + vy * c2) * 80 * 0.5f);
			}
		}
	}

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "W for Wander, F for Flee, S for Seek", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}