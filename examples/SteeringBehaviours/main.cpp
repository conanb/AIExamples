#include "SteeringBehavioursApp.h"
#include <iostream>

int main() {
	
	auto app = new SteeringBehavioursApp();
	app->run("AI", 1280, 720, false);
	delete app;

	std::cout << "Hi there Mr!" << std::endl;

	return 0;
}