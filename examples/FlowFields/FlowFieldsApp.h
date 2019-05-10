#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "SteeringBehaviour.h"
#include "Texture.h"

class FlowFieldsApp : public app::Application {
public:

	FlowFieldsApp();
	virtual ~FlowFieldsApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	app::Texture*		m_map;

	ai::Entity			m_entitys[100];

	ai::SteeringBehaviour	m_steeringBehaviour;
	ai::FlowForce			m_flowForce;
	
	enum eFlowFieldCosts : unsigned short {
		WALKABLE = 1,
		IMPASSABLE = 0xffff,
	};
	
	enum eFlowFieldSize {
		FLOWFIELD_ROWS = 23,
		FLOWFIELD_COLS = 40,
		FLOWFIELD_CELLSIZE = 32,
	};

	// contains movement costs through the field
	unsigned short	m_costField[FLOWFIELD_ROWS][FLOWFIELD_COLS];

	// contains actual travel cost to goal cell
	float			m_integrationField[FLOWFIELD_ROWS][FLOWFIELD_COLS];

	// the vector flow field that travels towards the goal cell
	glm::vec3			m_flowField[FLOWFIELD_ROWS][FLOWFIELD_COLS][1];

	// percentage of grid taken up by obstacles
	float m_obstaclePercentage = 0.15f;
	
	// randomly generates obstacles and places entities safely
	void randomiseLevel(float obstaclePercentage);

	// the neighbours parameter will be filled with 2x int for 
	// every neighbour ([y][x] within the field)
	// ignores impassable neighbours
	void getCellNeighbours(int x, int y, std::vector<int>& neighbours);

	// generate dijkstras to goal cell
	void performIntergration(int goalX, int goalY);

	// calculate flow based on gScores
	void generateFlowfield();

	// visualisation stuff
	float m_maxCost = 0;
	bool m_drawFlow = false;
	bool m_drawGradient = false;
	bool m_drawHSL = false;
};