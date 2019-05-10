#include "NavMeshApp.h"
#include "Font.h"
#include "Input.h"

#include "BehaviourTree.h"

NavMeshApp::NavMeshApp() {

}

NavMeshApp::~NavMeshApp() {

}

bool NavMeshApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	m_navMesh = new NavMesh(1280, 720);

	// just to help mess with the random
	srand(42);

	// random obstacles
	for (int i = 0; i < 12; ++i) {

		bool safe = false;
		do {
			safe = m_navMesh->addObstacle(rand() / float(RAND_MAX) * getWindowWidth() * 0.75f + getWindowWidth() * 0.125f,
										  rand() / float(RAND_MAX) * getWindowHeight() * 0.75f + getWindowHeight() * 0.125f,
										  60, 60,
										  10);
		} while (safe == false);
	}

	m_navMesh->build();

	auto start = m_navMesh->getRandomNode();
	auto end = start;
	while (end == start)
		end = m_navMesh->getRandomNode();

	graph::Search::aStar(start, end, m_path, NavMesh::Node::heuristic);

	NavMesh::smoothPath(m_path, m_smoothPath);

	m_player.getBlackboard().set("path", &m_path);
	m_player.getBlackboard().set("smoothpath", &m_smoothPath);
	m_player.getBlackboard().set("speed", 100.0f);
	m_player.setPosition({ start->position.x, start->position.y, 0 });

//	auto selector = new SelectorBehaviour();
//	selector->addChild(new NavMesh::FollowPathBehaviour());
//	selector->addChild(new NavMesh::NewPathBehaviour(m_navMesh));

	m_player.addBehaviour(new NavMesh::FollowPathBehaviour());

	return true;
}

void NavMeshApp::shutdown() {

	delete m_navMesh;

	delete m_font;
	delete m_2dRenderer;
}

void NavMeshApp::update() {
	
	m_player.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	if (input->wasMouseButtonPressed(app::INPUT_MOUSE_BUTTON_LEFT)) {

		int x = 0, y = 0;
		input->getMouseXY(&x, &y);

		auto end = m_navMesh->findClosest({ x, y, 0 });

		auto position = m_player.getPosition();

		auto start = m_navMesh->findClosest(position);

		if (start != end) {
			graph::Search::aStar(start, end, m_path, NavMesh::Node::heuristic);

			NavMesh::smoothPath(m_path, m_smoothPath);
		}
	}
}

void NavMeshApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	auto position = m_player.getPosition();
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	std::list<glm::vec3> smooth;
	NavMesh::smoothPath(m_path, smooth);

	m_2dRenderer->setRenderColour(1, 1, 1);
	auto prev = smooth.begin();
	for (auto iter = smooth.begin(); iter != smooth.end(); ++iter) {

		if (iter != smooth.begin()) {

			m_2dRenderer->drawLine(iter->x, iter->y, prev->x, prev->y, 3);
		}
		prev = iter;
	}

	m_2dRenderer->setRenderColour(0, 1, 1);
	for (auto node : m_path) {

		auto start = (NavMesh::Node*)node;
		auto end = (NavMesh::Node*)node->previous;

		if (end != nullptr) {
			m_2dRenderer->drawLine(start->position.x, start->position.y, end->position.x, end->position.y, 3);
		}
	}

	// draw nav mesh polygons
	for (auto node : m_navMesh->getNodes()) {

		m_2dRenderer->setRenderColour(1, 1, 0);
		m_2dRenderer->drawLine(node->vertices[0].x, node->vertices[0].y, node->vertices[1].x, node->vertices[1].y);
		m_2dRenderer->drawLine(node->vertices[1].x, node->vertices[1].y, node->vertices[2].x, node->vertices[2].y);
		m_2dRenderer->drawLine(node->vertices[2].x, node->vertices[2].y, node->vertices[0].x, node->vertices[0].y);
	}

	// draw obstacles
	m_2dRenderer->setRenderColour(1, 0, 0);
	for (auto& o : m_navMesh->getObstacles()) {
		m_2dRenderer->drawBox(o.x + o.w * 0.5f, o.y + o.h * 0.5f, o.w, o.h);
	}

	// done drawing sprites
	m_2dRenderer->end();
}