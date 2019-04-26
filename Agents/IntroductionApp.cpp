#include "IntroductionApp.h"
#include "Font.h"
#include "Input.h"

IntroductionApp::IntroductionApp() {

}

IntroductionApp::~IntroductionApp() {

}

bool IntroductionApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);

	m_keyboardBehaviour.setSpeed(400);

	m_followBehaviour.setSpeed(100);
	m_followBehaviour.setTarget(&m_player);

	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);

	m_player.addBehaviour(&m_keyboardBehaviour);
	m_enemy.addBehaviour(&m_followBehaviour);

	return true;
}

void IntroductionApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void IntroductionApp::update(float deltaTime) {

	m_player.update(deltaTime);
	m_enemy.update(deltaTime);

	// exit the application
	Input* input = Input::getInstance();
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();
}

void IntroductionApp::draw() {

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

	// draw some text
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "Use arrows for movement", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}