#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameObject.h"
#include "KeyboardBehaviour.h"
#include "FollowBehaviour.h"

class IntroductionApp : public Application {
public:

	IntroductionApp();
	virtual ~IntroductionApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	GameObject			m_player;
	KeyboardBehaviour	m_keyboardBehaviour;

	GameObject			m_enemy;
	FollowBehaviour		m_followBehaviour;
};