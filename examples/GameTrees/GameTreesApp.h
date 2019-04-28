#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameTreeBase.h"

class GameTreesApp : public Application {
public:

	GameTreesApp();
	virtual ~GameTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	enum {
		// game type
		TICTACTOE,
		CONNECTFOUR,

		// ai type
		RANDOMAI,
		MINIMAX,
		MONTECARLO,
	};

	int					m_gameType = CONNECTFOUR;
	int					m_aiType = MONTECARLO;

	GameTree::Game*		m_game;
	GameTree::AIPlayer*	m_ai;
};