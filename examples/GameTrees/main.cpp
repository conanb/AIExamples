#include "GameTreesApp.h"

int main() {
	
	auto app = new GameTreesApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}