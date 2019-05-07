#include "BasicApp.h"
#include "Font.h"
#include "Input.h"

BasicApp::BasicApp() {

}

BasicApp::~BasicApp() {

}

bool BasicApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("./font/consolas.ttf", 32);

	m_keyboardBehaviour.setSpeed(400);

	m_followBehaviour.setSpeed(100);
	m_followBehaviour.setTarget(&m_player);

	m_player.setPosition(glm::vec3(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.f));

	m_player.addBehaviour(&m_keyboardBehaviour);
	m_enemy.addBehaviour(&m_followBehaviour);

	return true;
}

void BasicApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BasicApp::update() {

	m_player.executeBehaviours();
	m_enemy.executeBehaviours();

	// exit the application
	app::Input* input = app::Input::getInstance();
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void BasicApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw player as a green circle
	auto position = m_player.getPosition();
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	// draw enemy as a red circle
	position = m_enemy.getPosition();
	m_2dRenderer->setRenderColour(1, 0, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "Use arrows for movement", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}