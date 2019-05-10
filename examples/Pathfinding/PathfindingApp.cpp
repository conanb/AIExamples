#include "PathfindingApp.h"
#include "Font.h"
#include "Input.h"
#include "Timing.h"
#include "BehaviourTree.h"

PathfindingApp::PathfindingApp() {

}

PathfindingApp::~PathfindingApp() {

}

bool PathfindingApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	// if the image fails to load then the level is drawn black/white
	//m_spriteSheet.load("../../bin/textures/magecity.png");

	// map is 1280x720 / 20x20
	m_map.load("../../bin/map/map1.png");

	auto pixels = m_map.getPixels();
	auto channels = m_map.getFormat();

	int windowHeight = getWindowHeight();

	// create nodes based on an image
	for (unsigned int x = 0; x < m_map.getWidth(); ++x) {
		for (unsigned int y = 0; y < m_map.getHeight(); ++y) {

			// what pixel are we up to?
			int index = (y * m_map.getWidth() + x) * channels;

			// ignore black pixels
			switch (channels) {
			case 1:	if (pixels[index + 0] == 0) continue;
				break;
			case 2:	if (pixels[index + 0] == 0 &&
						pixels[index + 1] == 0) continue;
				break;
			case 3:	if (pixels[index + 0] == 0 &&
						pixels[index + 1] == 0 &&
						pixels[index + 2] == 0) continue;
				break;
			};

			MyNode* node = new MyNode();
			node->x = float(x * 20 + 10);
			node->y = windowHeight - float(y * 20 + 10);

			m_nodes.push_back(node);
		}
	}

	// create edges
	for (auto a : m_nodes) {
		for (auto b : m_nodes) {
			if (a == b) continue;

			float x = b->x - a->x;
			float y = b->y - a->y;
			float sqrDist = x * x + y * y;

			if (sqrDist <= (30*30)) {
				graph::Edge* edge = new graph::Edge();
				edge->cost = sqrDist;
				edge->target = b;

				a->edges.push_back(edge);
			}
		}
	}

	// place random medkits
	for (int i = 0; i < 5; ++i) {
		MyNode* node = m_nodes[rand() % m_nodes.size()];
		node->flags |= MEDKIT;
	}

	// perform search for starting path
	auto start = m_nodes[rand() % m_nodes.size()];

	graph::Search::aStar(start,
							   m_nodes[rand() % m_nodes.size()],
							   m_path,
							   MyNode::heuristicDistanceSqr);

	/*Pathfinding::Search::dijkstra(start,
							   m_nodes[rand() % m_nodes.size()],
							   m_path);*/

	// an example searching for closest MEDKIT
	//Pathfinding::Search::dijkstraFindFlags(start, Pathfinding::Node::MEDKIT, m_path);

	m_player.getBlackboard().set("path", &m_path);
	m_player.getBlackboard().set("speed", 100.0f);
	m_player.setPosition({ start->x, start->y, 0 });

	// create tree
	auto selector = new ai::SelectorBehaviour();
	auto followPath = new FollowPathBehaviour();
	auto newPath = new NewPathBehaviour(m_nodes);

	auto logDecorator = new ai::LogDecorator(newPath, "Finding New Path");

	selector->addChild(followPath);
	selector->addChild(logDecorator);

	m_player.addBehaviour(selector);

	return true;
}

void PathfindingApp::shutdown() {

	for (auto n : m_nodes)
		delete n;

	delete m_font;
	delete m_2dRenderer;
}

void PathfindingApp::update() {

	m_player.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void PathfindingApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	auto pixels = m_map.getPixels();
	auto channels = m_map.getFormat();

	int windowHeight = getWindowHeight();

	// draw map using sprite sheet
	for (unsigned int x = 0; x < m_map.getWidth(); ++x) {
		for (unsigned int y = 0; y < m_map.getHeight(); ++y) {

			int index = (y * m_map.getWidth() + x) * channels;

			// example hard-coded for an RGB image
			if (pixels[index + 0] == 0 &&
				pixels[index + 1] == 0 &&
				pixels[index + 2] == 0) {

				// use blank
				if (m_spriteSheet.getPixels() != nullptr)
					m_2dRenderer->setUVRect(0, 1 / 45.f, 1 / 8.f, 1 / 45.f);
				else
					m_2dRenderer->setRenderColour(0, 0, 0);
			}
			else if (pixels[index + 0] == 255 &&
					 pixels[index + 1] == 255 &&
					 pixels[index + 2] == 255) {

				// use grass
				if (m_spriteSheet.getPixels() != nullptr)
					m_2dRenderer->setUVRect(0, 0, 1 / 8.f, 1 / 45.f);
			//	else
			//		m_2dRenderer->setRenderColour(1, 1, 1);
			}

			float posX = float(x * 20 + 10);
			float posY = windowHeight - float(y * 20 + 10);

			if (m_spriteSheet.getPixels() != nullptr)
				m_2dRenderer->drawSprite(&m_spriteSheet,
										 posX, posY,
										 20, 20,
										 0, 10);
			else
				m_2dRenderer->drawSprite(nullptr,
										 posX, posY,
										 20, 20,
										 0, 10);
		}
	}

	m_2dRenderer->setRenderColour(1, 0, 0);
	auto position = m_player.getPosition();
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	// draw graph
	for (auto node : m_nodes) {

		m_2dRenderer->setRenderColour(1, 0, 0);
		m_2dRenderer->drawBox(node->x, node->y, 4, 4);

		// draw edges
		m_2dRenderer->setRenderColour(1, 1, 0);
		for (auto edge : node->edges) {
			MyNode* target = (MyNode*)edge->target;

			m_2dRenderer->drawLine(node->x, node->y, target->x, target->y, 1, 1);
		}
	}

	// draw path
	m_2dRenderer->setRenderColour(0, 1, 1);
	for (auto node : m_path) {

		MyNode* s = (MyNode*)node;
		MyNode* e = (MyNode*)node->previous;

		if (e != nullptr) {
			m_2dRenderer->drawLine(s->x, s->y, e->x, e->y, 3, 0);
		}
	}

	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

ai::eBehaviourResult FollowPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<graph::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false ||
		path->empty())
		return ai::eBehaviourResult::FAILURE;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	auto position = entity->getPosition();

	// access first node we're heading towards
	MyNode* first = (MyNode*)path->front();

	// distance to first
	auto diff = glm::vec3(first->x, first->y,0) - position;

	// if not at the target then move towards it
	if (glm::dot(diff, diff) > 25) {
		

		// move to target (can overshoot!)
		entity->translate(glm::normalize(diff) * speed * app::Time::deltaTime());
	}
	else {
		// at the node, remove it and move to the next
		path->pop_front();
	}
	return ai::eBehaviourResult::SUCCESS;
}

MyNode* findClosest(float x, float y, std::vector<MyNode*>& nodes) {

	MyNode* closest = nullptr;
	float closestDist = 2000 * 2000;

	for (auto node : nodes) {

		float dist = (node->x - x) * (node->x - x) + (node->y - y) * (node->y - y);

		if (dist < closestDist) {
			closest = node;
			closestDist = dist;
		}
	}

	return closest;
}

ai::eBehaviourResult NewPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<graph::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false)
		return ai::eBehaviourResult::FAILURE;

	auto position = entity->getPosition();

	// random end node
	bool found = false;
	do {

		auto first = findClosest(position.x, position.y, m_nodes);
		auto end = m_nodes[rand() % m_nodes.size()];

		found = graph::Search::aStar(first, end, *path, MyNode::heuristicDistanceSqr);

	} while (found == false);


	return ai::eBehaviourResult::SUCCESS;
}