#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Random.h"
#include "Agent.h"
#include "SteeringBehaviour.h"

class FlockingApp : public app::Application {
public:

	FlockingApp();
	virtual ~FlockingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:
	
	app::Renderer2D*	m_2dRenderer;
	app::Font*			m_font;

	std::vector<ai::Agent>	m_entities;

	ai::SteeringBehaviour	m_steeringBehaviour;

	ai::WanderForce			m_wander;
	ai::SeparationForce		m_separation;
	ai::CohesionForce		m_cohesion;
	ai::AlignmentForce		m_alignment;

	app::Random				m_rand;
};