#include "FlockingApp.h"
#include "Font.h"
#include "Input.h"

FlockingApp::FlockingApp() {

}

FlockingApp::~FlockingApp() {

}

bool FlockingApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("./font/consolas.ttf", 32);
	
	m_entities.resize(200);

	m_separation.setEntities(&m_entities);
	m_separation.setRadius(100);

	m_cohesion.setEntities(&m_entities);
	m_cohesion.setRadius(100);

	m_alignment.setEntities(&m_entities);
	m_alignment.setRadius(100);

	m_steeringBehaviour.addForce(&m_wander, 1);
	m_steeringBehaviour.addForce(&m_separation, 1.25f);
	m_steeringBehaviour.addForce(&m_cohesion, 1);
	m_steeringBehaviour.addForce(&m_alignment, 1);

	for (auto& entity : m_entities) {

		float a = m_rand.nextReal() * 3.14159f * 2;

		glm::vec3* v = new glm::vec3();
		v->x = sinf(a) * 150;
		v->y = cosf(a) * 150;
		entity.getBlackboard().set("velocity", v, true);
		entity.getBlackboard().set("maxForce", 250.f);
		entity.getBlackboard().set("maxVelocity", 100.f);

		ai::WanderData* wd = new ai::WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->target = { 0 };
		wd->axisWeights = { 1, 1, 0 };
		entity.getBlackboard().set("wanderData", wd, true);

		entity.addBehaviour(&m_steeringBehaviour);

		entity.setPosition({ m_rand.nextReal() * getWindowWidth(),
			m_rand.nextReal() * getWindowHeight(), 0.0f });
	}

	return true;
}

void FlockingApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void FlockingApp::update() {

	for (auto& entity : m_entities)
		entity.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void FlockingApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	glm::vec3 position;

	for (auto& entity : m_entities) {

		position = entity.getPosition();
		screenWrap(position);
		entity.setPosition(position);

		m_2dRenderer->drawBox(position.x, position.y, 4, 4);
	}
	
	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}