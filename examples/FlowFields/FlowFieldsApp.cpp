#include "FlowFieldsApp.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

// helper colour tools
float hue2rgb(float p, float q, float t) {
	if (t < 0) t += 1;
	if (t > 1) t -= 1;
	if (t < 1 / 6.0f) return p + (q - p) * 6 * t;
	if (t < 1 / 2.0f) return q;
	if (t < 2 / 3.0f) return p + (q - p) * (2 / 3.0f - t) * 6;
	return p;
}
void hslToRgb(float h, float s, float l, float& r, float& g, float& b) {
	if (s == 0) {
		r = g = b = l; // achromatic
	}
	else {
		float q = l < 0.5f ? l * (1 + s) : l + s - l * s;
		float p = 2 * l - q;
		r = hue2rgb(p, q, h + 1 / 3.0f);
		g = hue2rgb(p, q, h);
		b = hue2rgb(p, q, h - 1 / 3.0f);
	}
}

FlowFieldsApp::FlowFieldsApp() {

}

FlowFieldsApp::~FlowFieldsApp() {

}

bool FlowFieldsApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("./font/consolas.ttf", 32);

	m_map = new app::Texture("./map/flowfield.png");

	// generate random field
	// (could instead generate from an image)
	randomiseLevel(m_obstaclePercentage);

	m_flowForce.setField(&(m_flowField[0][0][0]),
						 FLOWFIELD_ROWS, FLOWFIELD_COLS, 1,
						 FLOWFIELD_CELLSIZE);

	m_steeringBehaviour.addForce(&m_flowForce);

	for (auto& go : m_entitys) {

		go.addBehaviour(&m_steeringBehaviour);

		go.getBlackboard().set("velocity", new glm::vec3(0), true);
		go.getBlackboard().set("maxForce", 200.f);
		go.getBlackboard().set("maxVelocity", 50.f);
	}

	return true;
}

void FlowFieldsApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void FlowFieldsApp::update() {

	for (auto& go : m_entitys)
		go.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	// visualisation toggles
	if (input->wasKeyPressed(app::INPUT_KEY_G))
		m_drawGradient = !m_drawGradient;
	if (input->wasKeyPressed(app::INPUT_KEY_F))
		m_drawFlow = !m_drawFlow;
	if (input->wasKeyPressed(app::INPUT_KEY_C))
		m_drawHSL = !m_drawHSL;

	// randomise level
	glm::vec3* velocity = nullptr;
	if (input->wasKeyPressed(app::INPUT_KEY_R)) {
		randomiseLevel(m_obstaclePercentage);
		for (auto& go : m_entitys) {
			go.getBlackboard().get("velocity", &velocity);
			*velocity = glm::vec3(0);
		}
	}

	// pick goal cell
	if (input->isMouseButtonDown(app::INPUT_MOUSE_BUTTON_LEFT)) {

		// find cell under mouse
		int x = 0, y = 0;
		input->getMouseXY(&x, &y);

		// convert screen space to "grid space"
		x /= FLOWFIELD_CELLSIZE;
		y /= FLOWFIELD_CELLSIZE;

		// error!
		if (m_costField[y][x] != eFlowFieldCosts::IMPASSABLE) {
			
			performIntergration(x, y);
			generateFlowfield();
		}
	}
}

void FlowFieldsApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw obstacles
	for (int r = 0; r < FLOWFIELD_ROWS; ++r) {
		for (int c = 0; c < FLOWFIELD_COLS; ++c) {
			if (m_costField[r][c] == eFlowFieldCosts::IMPASSABLE) {
				m_2dRenderer->setRenderColour(1, 1, 1);
				m_2dRenderer->drawBox(FLOWFIELD_CELLSIZE * 0.5f + c * FLOWFIELD_CELLSIZE,
									  FLOWFIELD_CELLSIZE * 0.5f + r * FLOWFIELD_CELLSIZE,
									  FLOWFIELD_CELLSIZE, FLOWFIELD_CELLSIZE);
				m_2dRenderer->setRenderColour(1,0,0);
				m_2dRenderer->drawBox(FLOWFIELD_CELLSIZE * 0.5f + c * FLOWFIELD_CELLSIZE,
					FLOWFIELD_CELLSIZE * 0.5f + r * FLOWFIELD_CELLSIZE,
					FLOWFIELD_CELLSIZE * .75f, FLOWFIELD_CELLSIZE * .75f);
			}
			else {

				if (m_drawGradient) {

					// greyscale
					float colour = m_integrationField[r][c] / m_maxCost;

					// greyscale to rgb
					float red, green, blue;
					hslToRgb(colour, 1, 0.5f, red, green, blue);

					if (m_drawHSL)
						m_2dRenderer->setRenderColour(red, green, blue);
					else
						m_2dRenderer->setRenderColour(colour, colour, colour);

					m_2dRenderer->drawBox(FLOWFIELD_CELLSIZE * 0.5f + c * FLOWFIELD_CELLSIZE,
										  FLOWFIELD_CELLSIZE * 0.5f + r * FLOWFIELD_CELLSIZE,
										  FLOWFIELD_CELLSIZE, FLOWFIELD_CELLSIZE);
				}

				if (m_drawFlow) {
					m_2dRenderer->setRenderColour(1, 1, 0);
					m_2dRenderer->drawLine(FLOWFIELD_CELLSIZE * 0.5f + c * FLOWFIELD_CELLSIZE,
										   FLOWFIELD_CELLSIZE * 0.5f + r * FLOWFIELD_CELLSIZE,
										   FLOWFIELD_CELLSIZE * 0.5f + c * FLOWFIELD_CELLSIZE + m_flowField[r][c][0].x * 16,
										   FLOWFIELD_CELLSIZE * 0.5f + r * FLOWFIELD_CELLSIZE + m_flowField[r][c][0].y * 16);
				}
			}
		}
	}

	// draw game objects
	m_2dRenderer->setRenderColour(1, 1, 1);
	for (auto& go : m_entitys) {
		auto position = go.getPosition();
		m_2dRenderer->drawBox(position.x, position.y, 8, 8);
	}

	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawText(m_font, "G = toggle gradient, F = toggle flow, C = toggle colour", 0, 0, -1);
	
	// done drawing sprites
	m_2dRenderer->end();
}

void FlowFieldsApp::randomiseLevel(float obstaclePercentage) {

	// randomly place obstacles
	for (int r = 0; r < FLOWFIELD_ROWS; ++r) {
		for (int c = 0; c < FLOWFIELD_COLS; ++c) {			

			// a really bad random percentage calculator
			//if (rand() % 100 < int(100 * obstaclePercentage))

			if (m_map->getPixels()[r * FLOWFIELD_COLS + c] == 0)
				m_costField[r][c] = eFlowFieldCosts::IMPASSABLE;
			else
				m_costField[r][c] = eFlowFieldCosts::WALKABLE;

			m_integrationField[r][c] = 0;
			m_flowField[r][c][0].x = 0;
			m_flowField[r][c][0].y = 0;
			m_flowField[r][c][0].z = 0;
		}
	}

	// safely place game objects
	for (auto& go : m_entitys) {
		int index = 0;
		do {
			index = rand() % (FLOWFIELD_ROWS * FLOWFIELD_COLS);
		} while (m_costField[index / FLOWFIELD_COLS][index % FLOWFIELD_COLS] == eFlowFieldCosts::IMPASSABLE);

		go.setPosition({ FLOWFIELD_CELLSIZE * 0.5f + (index % FLOWFIELD_COLS) * FLOWFIELD_CELLSIZE,
					   FLOWFIELD_CELLSIZE * 0.5f + (index / FLOWFIELD_COLS) * FLOWFIELD_CELLSIZE, 0.0f });
	}
}

void FlowFieldsApp::getCellNeighbours(int x, int y, std::vector<int>& neighbours) {

	neighbours.clear();

	// down
	if (y > 0 &&
		m_costField[y - 1][x] != eFlowFieldCosts::IMPASSABLE) {
		neighbours.push_back(x);
		neighbours.push_back(y - 1);
	}
	// up
	if (y < (FLOWFIELD_ROWS - 1) &&
		m_costField[y + 1][x] != eFlowFieldCosts::IMPASSABLE) {
		neighbours.push_back(x);
		neighbours.push_back(y + 1);
	}
	// left
	if (x > 0 &&
		m_costField[y][x - 1] != eFlowFieldCosts::IMPASSABLE) {
		neighbours.push_back(x - 1);
		neighbours.push_back(y);

		// down diag
		if (y > 0 &&
			m_costField[y - 1][x - 1] != eFlowFieldCosts::IMPASSABLE &&
			m_costField[y - 1][x] != eFlowFieldCosts::IMPASSABLE) {
			neighbours.push_back(x - 1);
			neighbours.push_back(y - 1);
		}
		// up diag
		if (y < (FLOWFIELD_ROWS - 1) &&
			m_costField[y + 1][x - 1] != eFlowFieldCosts::IMPASSABLE &&
			m_costField[y + 1][x] != eFlowFieldCosts::IMPASSABLE) {
			neighbours.push_back(x - 1);
			neighbours.push_back(y + 1);
		}
	}
	// right
	if (x < (FLOWFIELD_COLS - 1) &&
		m_costField[y][x + 1] != eFlowFieldCosts::IMPASSABLE) {
		neighbours.push_back(x + 1);
		neighbours.push_back(y);

		// down diag
		if (y > 0 &&
			m_costField[y - 1][x + 1] != eFlowFieldCosts::IMPASSABLE &&
			m_costField[y - 1][x] != eFlowFieldCosts::IMPASSABLE) {
			neighbours.push_back(x + 1);
			neighbours.push_back(y - 1);
		}
		// up diag
		if (y < (FLOWFIELD_ROWS - 1) &&
			m_costField[y + 1][x + 1] != eFlowFieldCosts::IMPASSABLE &&
			m_costField[y + 1][x] != eFlowFieldCosts::IMPASSABLE) {
			neighbours.push_back(x + 1);
			neighbours.push_back(y + 1);
		}
	}

	// 8 potential neighbours
	// This creates diagonals through corners with impassable
	/*for (int r = -1; r <= 1; ++r) {
		for (int c = -1; c <= 1; ++c) {
			if (x == c &&
				y == r)
				continue;

			if ((x + c) >= 0 &&
				(x + c) <= (FLOWFIELD_COLS - 1) &&
				(y + r) >= 0 &&
				(y + r) <= (FLOWFIELD_ROWS - 1) &&
				m_costField[y + r][x + c] != eFlowFieldCosts::IMPASSABLE) {

				// but can we reach it?
				neighbours.push_back(x + c);
				neighbours.push_back(y + r);
			}
		}
	}*/
}

void FlowFieldsApp::performIntergration(int goalX, int goalY) {
	// perform a dijkstra's search towards goal cell, 
	// starting from goal cell spreading outwards

	// reset field
	for (int r = 0; r < FLOWFIELD_ROWS; ++r) {
		for (int c = 0; c < FLOWFIELD_COLS; ++c) {
			m_integrationField[r][c] = FLT_MAX;
		}
	}

	// start open list
	std::list<std::pair<int, int>> openList;
	openList.push_back({ goalX, goalY });

	m_integrationField[goalY][goalX] = 0;

	std::vector<int> neighbours(16);

	m_maxCost = 0;

	// do reverse dijkstra
	while (openList.empty() == false) {

		auto current = openList.front();
		openList.pop_front();

		int x = current.first;
		int y = current.second;

		getCellNeighbours(x, y, neighbours);
		
		unsigned int neighbourCount = neighbours.size();

		// add neighbours to open list
		for (unsigned int i = 0; i < neighbourCount; i += 2) {

			int nx = neighbours[i];
			int ny = neighbours[i + 1];

			float modifier = 1;

			if (x != nx &&
				y != ny)
				modifier = 1.5f;

			float cost = m_integrationField[y][x] + m_costField[ny][nx] * modifier;

			if (cost > m_maxCost)
				m_maxCost = cost;

			if (cost < m_integrationField[ny][nx]) {

				m_integrationField[ny][nx] = cost;

				std::pair<int, int> neighbour = { nx, ny };

				// is neighbour on open list?
				auto iter = std::find(openList.begin(), openList.end(), neighbour);
				if (iter == openList.end())
					openList.push_back(neighbour);
			}
		}
	}
}

void FlowFieldsApp::generateFlowfield() {
	// for each grid cell, sample neighbour costs and 
	// setup flow direction to lowest cost neighbour

	std::vector<int> neighbours(16);
	int x, y; // lowest X and Y for the neighbour
	float lowestCost;

	for (int r = 0; r < FLOWFIELD_ROWS; ++r) {
		for (int c = 0; c < FLOWFIELD_COLS; ++c) {

			m_flowField[r][c][0].x = 0;
			m_flowField[r][c][0].y = 0;
			m_flowField[r][c][0].z = 0;

			lowestCost = FLT_MAX;

			getCellNeighbours(c, r, neighbours);

			unsigned int neighbourCount = neighbours.size();

			if (neighbourCount > 0) {

				// find bet neighbour
				for (unsigned int i = 0; i < neighbourCount; i += 2) {

					int nx = neighbours[i];
					int ny = neighbours[i + 1];

					if (m_integrationField[ny][nx] <= lowestCost) {
						lowestCost = m_integrationField[ny][nx];
						x = nx;
						y = ny;
					}
				}

				float mag = float((x - c) * (x - c) + (y - r) * (y - r));

				if (mag > 0) {
					mag = sqrt(mag);

					m_flowField[r][c][0].x = (x - c) / mag;
					m_flowField[r][c][0].y = (y - r) / mag;
					m_flowField[r][c][0].z = 0;
				}
			}
		}
	}
}