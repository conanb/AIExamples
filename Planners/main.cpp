#include "PlannersApp.h"

int main() {
	
	auto app = new PlannersApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}