#include "AIShowcaseApp.h"

int main() {
	
	auto app = new AIShowcaseApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}