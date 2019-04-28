#include "BasicApp.h"

int main() {
	
	auto app = new BasicApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}