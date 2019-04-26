#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "KeyboardBehaviour.h"
#include "BehaviourTree.h"
#include "SteeringBehaviour.h"

class BehaviourTreesApp : public Application {
public:

	BehaviourTreesApp();
	virtual ~BehaviourTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*			m_font;

	void screenWrap(float& x, float& y);

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	GameObject			m_enemy[5];
	Behaviour*			m_guardBehaviour;

	std::vector<Obstacle>	m_obstacles;
};