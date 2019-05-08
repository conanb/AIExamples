#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "Entity.h"
#include "KeyboardBehaviour.h"
#include "State.h"

class FiniteStateMachineApp : public app::Application {
public:

	FiniteStateMachineApp();
	virtual ~FiniteStateMachineApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	ai::Entity			m_player;
	ai::KeyboardBehaviour	m_keyboardBehaviour;

	ai::Entity			m_enemy;
	ai::FiniteStateMachine	m_guardFSM;
};