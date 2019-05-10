#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "KeyboardBehaviour.h"
#include "SteeringBehaviour.h"

class SteeringBehavioursApp : public app::Application {
public:

	SteeringBehavioursApp();
	virtual ~SteeringBehavioursApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	ai::Entity			m_player;
	ai::KeyboardBehaviour	m_keyboardBehaviour;

	ai::Entity			m_enemies[10];
	ai::FiniteStateMachine	m_fsm;

	// keeping access so that I can toggle weights
	ai::SteeringState*		m_attackState;

	ai::SeekForce				m_seek;
	ai::FleeForce				m_flee;
	ai::WanderForce				m_wander;
	//ObstacleAvoidanceForce	m_avoid;

	//std::vector<Obstacle>	m_obstacles;
};