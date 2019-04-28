#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "SteeringBehaviour.h"

class FlockingApp : public Application {
public:

	FlockingApp();
	virtual ~FlockingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	void screenWrap(float& x, float& y);

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	std::vector<Entity>	m_entities;

	SteeringBehaviour	m_steeringBehaviour;

	WanderForce			m_wander;
	SeparationForce		m_separation;
	CohesionForce		m_cohesion;
	AlignmentForce		m_alignment;

};