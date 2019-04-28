#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "KeyboardBehaviour.h"
#include "State.h"

class FiniteStateMachineApp : public Application {
public:

	FiniteStateMachineApp();
	virtual ~FiniteStateMachineApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	Entity			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	Entity			m_enemy;
	FiniteStateMachine	m_guardFSM;
};