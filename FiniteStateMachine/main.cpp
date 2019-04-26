#include "FiniteStateMachineApp.h"

int main() {
	
	auto app = new FiniteStateMachineApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}