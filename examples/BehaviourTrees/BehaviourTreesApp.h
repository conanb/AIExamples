#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "KeyboardBehaviour.h"
#include "BehaviourTree.h"
#include "SteeringBehaviour.h"

class BehaviourTreesApp : public app::Application {
public:

	BehaviourTreesApp();
	virtual ~BehaviourTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;
	
	ai::Agent				m_player;
	ai::KeyboardBehaviour	m_keyboardBehaviour;

	ai::Agent			m_enemy[5];
	ai::Behaviour*		m_guardBehaviour;

//	std::vector<Obstacle>	m_obstacles;
};