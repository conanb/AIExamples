#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameObject.h"
#include "KeyboardBehaviour.h"
#include "SteeringBehaviour.h"

class SteeringBehavioursApp : public Application {
public:

	SteeringBehavioursApp();
	virtual ~SteeringBehavioursApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void screenWrap(float& x, float& y);

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	GameObject			m_enemies[10];
	FiniteStateMachine	m_fsm;

	// keeping access so that I can toggle weights
	SteeringState*		m_attackState;

	SeekForce				m_seek;
	FleeForce				m_flee;
	WanderForce				m_wander;
	ObstacleAvoidanceForce	m_avoid;

	std::vector<Obstacle>	m_obstacles;
};