#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "KeyboardBehaviour.h"
#include "Decision.h"
#include "SteeringBehaviour.h"

class AttackDecision : public ai::Decision {
public:

	AttackDecision() {}
	virtual ~AttackDecision() {}

	virtual void makeDecision(ai::Entity* entity) {}
};

class DecisionTreesApp : public app::Application {
public:

	DecisionTreesApp();
	virtual ~DecisionTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	void screenWrap(glm::vec3& p);

	ai::Entity			m_player;
	ai::KeyboardBehaviour	m_keyboardBehaviour;

	ai::Entity			m_enemy;
	ai::DecisionBehaviour	m_enemyDecisions;

//	std::vector<Obstacle>	m_obstacles;
};