#include "BehaviourTreesApp.h"
#include "Font.h"
#include "Input.h"

#include "SteeringBehaviour.h"

BehaviourTreesApp::BehaviourTreesApp() {

}

BehaviourTreesApp::~BehaviourTreesApp() {

}

bool BehaviourTreesApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.addBehaviour(&m_keyboardBehaviour);
	m_player.setPosition({ getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f });

	// obstacle avoidance force used by decisions
	auto obstacleForce = new ai::ObstacleAvoidanceForce();
	obstacleForce->setFeelerLength(80);

	// aimless wandering forces
	auto wanderingBehaviour = new ai::SteeringBehaviour();
	wanderingBehaviour->addForce(new ai::WanderForce());
	wanderingBehaviour->addForce(obstacleForce);

	// attacking steering forces
	auto attackingBehaviour = new ai::SteeringBehaviour();
	attackingBehaviour->addForce(new ai::SeekForce(&m_player), 0.8f);
	attackingBehaviour->addForce(obstacleForce);

	// conditions
	auto within200Condition = new ai::WithinRangeCondition(&m_player, 200);
	auto within50Condition = new ai::WithinRangeCondition(&m_player, 50);

	// behaviour tree branches
	auto guardBehaviour = new ai::SelectorBehaviour();
	auto seekBehaviour = new ai::SequenceBehaviour();

	// structure the tree
	guardBehaviour->addChild(within50Condition);

	guardBehaviour->addChild(seekBehaviour);

		// AND sequence
		seekBehaviour->addChild(within200Condition);
		seekBehaviour->addChild(attackingBehaviour);
		
	guardBehaviour->addChild(wanderingBehaviour);

	// attach behaviour
	m_guardBehaviour = guardBehaviour;

	// setup enemy
	for (auto& enemy : m_enemy) {

		enemy.addBehaviour(m_guardBehaviour);
		enemy.setPosition({ 50, 50,0 });

		// add some steering data to the blackboard
		enemy.getBlackboard().set("maxForce", 300.f);
		enemy.getBlackboard().set("maxVelocity", 150.f);
		enemy.getBlackboard().set("velocity", new glm::vec3(0), true);
		enemy.getBlackboard().set("wanderData", new ai::WanderData({ 100.0f, 75.0f, 25.0f, glm::vec3(0), glm::vec3(1,1,0) }), true);
	}
	
	// set up my obstacles
	for (int i = 0; i < 10; ++i) {

		ai::Obstacle c;
		c.type = ai::Obstacle::SPHERE;
		c.center.x = rand() % (getWindowWidth() - 150) + 75.f;
		c.center.y = rand() % (getWindowHeight() - 150) + 75.f;
		c.center.z = 0;
		c.radius = rand() % 40 + 40.f;

		m_obstacles.push_back(c);

		obstacleForce->addSphereObstacle(c.center.x, c.center.y, c.center.z, c.radius);
	}
	return true;
}

void BehaviourTreesApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BehaviourTreesApp::update() {

	m_player.executeBehaviours();

	for (auto& enemy : m_enemy)
		enemy.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void BehaviourTreesApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw obstacles as pink circles
	m_2dRenderer->setRenderColour(1, 0, 1);
	for (auto circle : m_obstacles) {
		m_2dRenderer->drawCircle(circle.center.x, circle.center.y, circle.radius);
	}
	
	// draw player as a green circle
	auto position = m_player.getPosition();
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	screenWrap(position);
	m_player.setPosition(position);

	// draw enemy as a red circle
	m_2dRenderer->setRenderColour(1, 0, 0);
	for (auto& enemy : m_enemy) {

		position = enemy.getPosition();
		screenWrap(position);

		m_2dRenderer->drawCircle(position.x, position.y, 10);
		enemy.setPosition(position);
	}

	// draw attack radius
	m_2dRenderer->setRenderColour(1, 1, 0, 0.2f);
	for (auto& enemy : m_enemy) {
		position = enemy.getPosition();
		m_2dRenderer->drawCircle(position.x, position.y, 50);
	}

	// draw seek radius
	m_2dRenderer->setRenderColour(0, 1, 0, 0.2f);
	for (auto& enemy : m_enemy) {
		position = enemy.getPosition();
		m_2dRenderer->drawCircle(position.x, position.y, 200);
	}

	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}