#include "FlowFieldsApp.h"

int main() {
	
	auto app = new FlowFieldsApp();
	app->run("AI", 1280, 720, false);
	delete app;

	return 0;
}