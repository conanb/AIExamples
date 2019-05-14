#include "KeyboardBehaviour.h"
#include "Input.h"
#include "Timing.h"

namespace ai {

KeyboardBehaviour::KeyboardBehaviour()
	: m_speed(1) {
}

KeyboardBehaviour::~KeyboardBehaviour() {

}

eBehaviourResult KeyboardBehaviour::execute(Agent* entity) {

	float x = 0, y = 0;

	// get access to input
	app::Input* input = app::Input::getInstance();

	// determine direction to move
	if (input->isKeyDown(app::INPUT_KEY_UP))
		y += 1;

	if (input->isKeyDown(app::INPUT_KEY_DOWN))
		y -= 1;

	if (input->isKeyDown(app::INPUT_KEY_LEFT))
		x -= 1;

	if (input->isKeyDown(app::INPUT_KEY_RIGHT))
		x += 1;

	// we need to adjust the direction when heading diagonally
	float magnitude = sqrt(x * x + y * y);
	if (magnitude > 0) {
		x /= magnitude;
		y /= magnitude;
	}

	// apply the movement based on speed and delta time
	entity->translate({ x * m_speed * app::Time::deltaTime(), y * m_speed * app::Time::deltaTime(), 0 });

	return eBehaviourResult::SUCCESS;
}

} // namespace ai