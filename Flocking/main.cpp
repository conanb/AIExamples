#include "FlockingApp.h"

int main() {
	
	auto app = new FlockingApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}