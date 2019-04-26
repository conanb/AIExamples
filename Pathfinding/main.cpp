#include "PathfindingApp.h"

int main() {
	
	auto app = new PathfindingApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}