#include "KeyboardBehaviour.h"
#include "Input.h"

KeyboardBehaviour::KeyboardBehaviour()
	: m_speed(1) {
}

KeyboardBehaviour::~KeyboardBehaviour() {

}

eBehaviourResult KeyboardBehaviour::execute(GameObject* gameObject, float deltaTime) {

	float x = 0, y = 0;

	// get access to input
	aie::Input* input = aie::Input::getInstance();

	// determine direction to move
	if (input->isKeyDown(aie::INPUT_KEY_UP))
		y += 1;

	if (input->isKeyDown(aie::INPUT_KEY_DOWN))
		y -= 1;

	if (input->isKeyDown(aie::INPUT_KEY_LEFT))
		x -= 1;

	if (input->isKeyDown(aie::INPUT_KEY_RIGHT))
		x += 1;

	// we need to adjust the direction when heading diagonally
	float magnitude = sqrt(x * x + y * y);
	if (magnitude > 0) {
		x /= magnitude;
		y /= magnitude;
	}

	// apply the movement based on speed and delta time
	gameObject->translate(x * m_speed * deltaTime, y * m_speed * deltaTime);

	return eBehaviourResult::SUCCESS;
}