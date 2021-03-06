#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include <string>
#include <vector>

class RouletteWheelSelectionApp : public app::Application {
public:

	RouletteWheelSelectionApp();
	virtual ~RouletteWheelSelectionApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	struct RouletteChance {
		float value;
		float modifier;

		unsigned int colour;
		std::string name;

		void action();

		static std::string lastSelected;
	};

	std::vector<RouletteChance>	m_needs;

	float m_total = 0;
};