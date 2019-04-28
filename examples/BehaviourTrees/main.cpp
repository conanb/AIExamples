#include "BehaviourTreesApp.h"

int main() {
	
	auto app = new BehaviourTreesApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}