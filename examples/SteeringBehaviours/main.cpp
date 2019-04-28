#include "SteeringBehavioursApp.h"

int main() {
	
	auto app = new SteeringBehavioursApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}