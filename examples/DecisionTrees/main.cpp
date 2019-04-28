#include "DecisionTreesApp.h"

int main() {
	
	auto app = new DecisionTreesApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}