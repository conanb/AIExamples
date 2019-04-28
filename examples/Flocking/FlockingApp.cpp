#include "FlockingApp.h"
#include "Font.h"
#include "Input.h"

FlockingApp::FlockingApp() {

}

FlockingApp::~FlockingApp() {

}

bool FlockingApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);
	
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

		float a = rand() / (float)RAND_MAX * 3.14159f * 2;

		Vector2* v = new Vector2();
		v->x = sinf(a) * 150;
		v->y = cosf(a) * 150;
		entity.getBlackboard().set("velocity", v, true);
		entity.getBlackboard().set("maxForce", 250.f);
		entity.getBlackboard().set("maxVelocity", 100.f);

		WanderData* wd = new WanderData();
		wd->offset = 100;
		wd->radius = 75;
		wd->jitter = 25;
		wd->x = 0;
		wd->y = 0;
		entity.getBlackboard().set("wanderData", wd, true);

		entity.addBehaviour(&m_steeringBehaviour);

		entity.setPosition(rand() / (float)RAND_MAX * getWindowWidth(),
						   rand() / (float)RAND_MAX * getWindowHeight());
	}

	return true;
}

void FlockingApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void FlockingApp::update(float deltaTime) {
	
	// HACK: can get laggy so just limiting dt for now
	deltaTime = 1 / 60.f;

	for (auto& entity : m_entities)
		entity.executeBehaviours(deltaTime);

	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();
}

void FlockingApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float x, y;

	for (auto& entity : m_entities) {

		entity.getPosition(&x, &y);
		screenWrap(x, y);
		entity.setPosition(x, y);

		m_2dRenderer->drawBox(x, y, 4, 4);
	}
	
	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

void FlockingApp::screenWrap(float& x, float& y) {
	// wrap position around the screen
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();
	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}