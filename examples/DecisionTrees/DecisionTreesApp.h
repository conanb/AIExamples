#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "KeyboardBehaviour.h"
#include "Decision.h"
#include "SteeringBehaviour.h"

class AttackDecision : public Decision {
public:

	AttackDecision() {}
	virtual ~AttackDecision() {}

	virtual void makeDecision(Entity* entity, float deltaTime) {}
};

class DecisionTreesApp : public Application {
public:

	DecisionTreesApp();
	virtual ~DecisionTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	void screenWrap(float& x, float& y);

	Entity			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	Entity			m_enemy;
	DecisionBehaviour	m_enemyDecisions;

	std::vector<Obstacle>	m_obstacles;
};