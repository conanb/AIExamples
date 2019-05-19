#include "DecisionTreesApp.h"
#include "Font.h"
#include "Input.h"

DecisionTreesApp::DecisionTreesApp() {

}

DecisionTreesApp::~DecisionTreesApp() {

}

bool DecisionTreesApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	// setup player
	m_keyboardBehaviour.setSpeed(400);
	m_player.addBehaviour(&m_keyboardBehaviour);
	m_player.setPosition({ getWindowWidth() * 0.5f, getWindowHeight() * 0.5f, 0.0f });

	// setup enemy
	m_enemy.addBehaviour(&m_enemyDecisions);
	m_enemy.setPosition(glm::vec3(50,50,0));

	// add some steering data to the blackboard
	m_enemy.getBlackboard().set("maxForce", 300.f);
	m_enemy.getBlackboard().set("maxVelocity", 150.f);
	m_enemy.getBlackboard().set("velocity", new glm::vec3(0), true);
	m_enemy.getBlackboard().set("wanderData", new ai::WanderData({ 200.0f, 75.0f, 25.0f, glm::vec3(0), glm::vec3(1,1,0) }), true);

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

	// decision tree branches
	auto attackOrSeekBranch = new ai::ConditionalDecision();
	auto rootBranch = new ai::ConditionalDecision();

	// set root branch
	m_enemyDecisions.setDecision(rootBranch);

	// construct the tree
	rootBranch->setCondition(new ai::WithinRangeCondition(&m_player, 200));
		// on true
		rootBranch->setTrueBranch(attackOrSeekBranch);
			attackOrSeekBranch->setCondition(new ai::WithinRangeCondition(&m_player, 50));
				// on true
				attackOrSeekBranch->setTrueBranch(new AttackDecision());
				// on false
				attackOrSeekBranch->setFalseBranch(new ai::BehaviourDecision(attackingBehaviour));
		// on false
		rootBranch->setFalseBranch(new ai::BehaviourDecision(wanderingBehaviour));
	
	// set up my obstacles
	for (int i = 0; i < 10; ++i) {

		ai::Obstacle c;
		c.type = ai::Obstacle::SPHERE;
		c.center.x = rand() % (getWindowWidth() - 150) + 75.f;
		c.center.y = rand() % (getWindowHeight() - 150) + 75.f;
		c.center.z = 0;
		c.radius = rand() % 40 + 40.f;

		m_obstacles.push_back(c);

		// add obstacles to avoid behaviour
		obstacleForce->addSphereObstacle(c.center.x, c.center.y, c.center.z, c.radius);
	}

	return true;
}

void DecisionTreesApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void DecisionTreesApp::update() {

	m_player.executeBehaviours();
	m_enemy.executeBehaviours();

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();
}

void DecisionTreesApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw obstacles as pink circles
	for (auto circle : m_obstacles) {
		m_2dRenderer->setRenderColour(1, 0, 1);
		m_2dRenderer->drawCircle(circle.center.x, circle.center.y, circle.radius);
	}

	// draw player as a green circle
	auto position = m_player.getPosition();
	m_2dRenderer->setRenderColour(0, 1, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	screenWrap(position);
	m_player.setPosition(position);

	// draw enemy as a red circle
	position = m_enemy.getPosition();
	m_2dRenderer->setRenderColour(1, 0, 0);
	m_2dRenderer->drawCircle(position.x, position.y, 10);

	screenWrap(position);
	m_enemy.setPosition(position);

	// draw attack radius
	m_2dRenderer->setRenderColour(1, 1, 0, 0.2f);
	m_2dRenderer->drawCircle(position.x, position.y, 50);

	// draw seek radius
	m_2dRenderer->setRenderColour(0, 1, 0, 0.2f);
	m_2dRenderer->drawCircle(position.x, position.y, 200);

	// output some text
	m_2dRenderer->drawText(m_font, "Press ESC to quit", 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}