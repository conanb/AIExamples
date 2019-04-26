#include "RouletteWheelSelectionApp.h"

int main() {
	
	auto app = new RouletteWheelSelectionApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}