#include "IntroductionApp.h"

int main() {
	
	auto app = new IntroductionApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}