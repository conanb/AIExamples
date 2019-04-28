#include "BlackboardsApp.h"

int main() {
	
	auto app = new BlackboardsApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}