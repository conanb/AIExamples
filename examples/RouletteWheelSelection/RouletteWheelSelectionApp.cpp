#include "RouletteWheelSelectionApp.h"
#include "Font.h"
#include "Input.h"
#include "Timing.h"
#include <glm/ext.hpp>

std::string RouletteWheelSelectionApp::RouletteChance::lastSelected = "None";

RouletteWheelSelectionApp::RouletteWheelSelectionApp() {

}

RouletteWheelSelectionApp::~RouletteWheelSelectionApp() {
}

void RouletteWheelSelectionApp::RouletteChance::action() {
	lastSelected = name;
}

bool RouletteWheelSelectionApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("./font/consolas.ttf", 32);

	m_needs.push_back({ 0.0f, 1.0f, 0xff0000ff, "Hungry" });
	m_needs.push_back({ 0.0f, 1.5f, 0xff00ffff, "Thirsty" });
	m_needs.push_back({ 0.0f, 0.6f, 0x0000ffff, "Angry" });
	m_needs.push_back({ 0.0f, 0.8f, 0x00ff00ff, "Tired" });
	m_needs.push_back({ 0.0f, 0.2f, 0xff99ffff, "Sad" });
	m_needs.push_back({ 0.0f, 0.75f, 0xffff00ff, "Rowdy" });
	m_needs.push_back({ 0.0f, 1.0f, 0x00ffffff, "Bored" });

	return true;
}

void RouletteWheelSelectionApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void RouletteWheelSelectionApp::update() {

	// increase all needs based on modifiers over time
	for (auto& need : m_needs) {
		need.value += app::Time::deltaTime() * need.modifier;
		m_total += app::Time::deltaTime() * need.modifier;
	}

	// HACK: select a need every 3 seconds
	static float selectionTimer = 0;
	selectionTimer += app::Time::deltaTime();
	if (selectionTimer >= 3.0f) {

		selectionTimer -= 3.0f;

		// selection
		float selection = glm::linearRand(0.0f, m_total);

		for (auto& need : m_needs) {
			if (selection < need.value) {
				m_total -= need.value;
				need.value = 0;
				need.action();
				break;
			}
			else
				selection -= need.value;
		}
	}

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void RouletteWheelSelectionApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	float total = 0;
	for (auto& need : m_needs)
		total += need.value;
	
	m_2dRenderer->setRenderColour(1,1,1);
	m_2dRenderer->drawLine(0, 662, getWindowWidth(), 662, 2);
	m_2dRenderer->drawLine(0, 638, getWindowWidth(), 638, 2);

	float x = 0;
	for (auto& need : m_needs) {
		m_2dRenderer->setRenderColour(need.colour);
		float width = need.value / total * getWindowWidth();
		m_2dRenderer->drawBox(x + width * 0.5f, 650, width, 24);
		x += width;
	}

	// draw your stuff here!
	float size = 1280.f / m_needs.size();
	int index = 0;
	for (auto& need : m_needs) {
		m_2dRenderer->setRenderColour(need.colour);
		m_2dRenderer->drawBox(index * size + size * 0.5f, 0, size, need.value * 50);
		m_2dRenderer->drawText(m_font, need.name.c_str(), index * size, need.value * 25);
		index++;
	}

	// output some text
	m_2dRenderer->drawText(m_font, "Last selected:", 0, 720 - 32);
	m_2dRenderer->drawText(m_font, RouletteChance::lastSelected.c_str(), 260, 720 - 32);

	// done drawing sprites
	m_2dRenderer->end();
}