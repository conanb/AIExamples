#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include "GameTreeBase.h"

class GameTreesApp : public app::Application {
public:

	GameTreesApp();
	virtual ~GameTreesApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

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