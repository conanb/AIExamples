#include "BehaviourTreesApp.h"
#include "Font.h"
#include "Input.h"

#include "SteeringBehaviour.h"

BehaviourTreesApp::BehaviourTreesApp() {

}

BehaviourTreesApp::~BehaviourTreesApp() {

}

bool BehaviourTreesApp::startup() {
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);

	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.addBehaviour(&m_keyboardBehaviour);
	m_player.setPosition(getWindowWidth() * 0.5f, getWindowHeight() * 0.5f);

	// obstacle avoidance force used by decisions
	auto obstacleForce = new ObstacleAvoidanceForce();
	obstacleForce->setFeelerLength(80);

	// aimless wandering forces
	auto wanderingBehaviour = new SteeringBehaviour();
	wanderingBehaviour->addForce(new WanderForce());
	wanderingBehaviour->addForce(obstacleForce);

	// attacking steering forces
	auto attackingBehaviour = new SteeringBehaviour();
	attackingBehaviour->addForce(new SeekForce(&m_player), 0.8f);
	attackingBehaviour->addForce(obstacleForce);

	// conditions
	auto within200Condition = new WithinRangeCondition(&m_player, 200);
	auto within50Condition = new WithinRangeCondition(&m_player, 50);

	// behaviour tree branches
	auto guardBehaviour = new SelectorBehaviour();
	auto seekBehaviour = new SequenceBehaviour();

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
		enemy.setPosition(50, 50);

		// add some steering data to the blackboard
		enemy.getBlackboard().set("maxForce", 300.f);
		enemy.getBlackboard().set("maxVelocity", 150.f);
		enemy.getBlackboard().set("velocity", new Vector2({ 0,0 }), true);
		enemy.getBlackboard().set("wanderData", new WanderData({ 100, 75, 25, 0, 0 }), true);
	}
	
	// set up my obstacles
	for (int i = 0; i < 10; ++i) {

		Obstacle c;
		c.type = Obstacle::SPHERE;
		c.x = rand() % (getWindowWidth() - 150) + 75.f;
		c.y = rand() % (getWindowHeight() - 150) + 75.f;
		c.r = rand() % 40 + 40.f;

		m_obstacles.push_back(c);

		obstacleForce->addSphereObstacle(c.x, c.y, c.r);
	}
	return true;
}

void BehaviourTreesApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void BehaviourTreesApp::update(float deltaTime) {

	m_player.executeBehaviours(deltaTime);

	for (auto& enemy : m_enemy)
		enemy.executeBehaviours(deltaTime);

	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
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
		m_2dRenderer->drawCircle(circle.x, circle.y, circle.r);
	}

	float x = 0, y = 0;

	// draw player as a green circle
	m_player.getPosition(&x, &y);
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(x, y, 10);

	screenWrap(x, y);
	m_player.setPosition(x, y);

	// draw enemy as a red circle
	m_2dRenderer->setRenderColour(1, 0, 0);
	for (auto& enemy : m_enemy) {

		enemy.getPosition(&x, &y);
		screenWrap(x, y);

		m_2dRenderer->drawCircle(x, y, 10);
		enemy.setPosition(x, y);
	}

	// draw attack radius
	m_2dRenderer->setRenderColour(1, 1, 0, 0.2f);
	for (auto& enemy : m_enemy) {
		enemy.getPosition(&x, &y);
		m_2dRenderer->drawCircle(x, y, 50);
	}

	// draw seek radius
	m_2dRenderer->setRenderColour(0, 1, 0, 0.2f);
	for (auto& enemy : m_enemy) {
		enemy.getPosition(&x, &y);
		m_2dRenderer->drawCircle(x, y, 200);
	}

	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

void BehaviourTreesApp::screenWrap(float& x, float& y) {
	// wrap position around the screen
	x = fmod(x, (float)getWindowWidth());
	if (x < 0)
		x += getWindowWidth();
	y = fmod(y, (float)getWindowHeight());
	if (y < 0)
		y += getWindowHeight();
}