#include "GameTreesApp.h"
#include "Font.h"
#include "Input.h"

#include "ConnectFourGame.h"
#include "TicTacToeGame.h"
#include "MiniMaxAI.h"
#include "MonteCarloAI.h"

GameTreesApp::GameTreesApp() {

}

GameTreesApp::~GameTreesApp() {

}

bool GameTreesApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	switch (m_gameType) {
	case TICTACTOE:		m_game = new TicTacToeGame();	break;
	case CONNECTFOUR:	m_game = new ConnectFourGame();	break;
	}

	switch (m_aiType) {
	case RANDOMAI:		m_ai = new GameTree::RandomAI();	break;
	case MINIMAX:		m_ai = new MiniMaxAI(GameTree::PLAYER_TWO);	break;
	case MONTECARLO:	m_ai = new MonteCarloAI(50);	break;
	};	

	return true;
}

void GameTreesApp::shutdown() {

	delete m_game;
	delete m_ai;
	delete m_font;
	delete m_2dRenderer;
}

void GameTreesApp::update() {
	
	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	// keep track of if the mouse has been clicked
	static bool moveMade = false;

	// if the game isn't over...
	if (m_game->getCurrentPlayState() == GameTree::ePlayState::UNKNOWN) {

		// if it is the user's turn (player 1)
		if (m_game->getCurrentPlayer() == GameTree::ePlayState::PLAYER_ONE) {

			// if the mouse button is down...
			if (input->wasMouseButtonPressed(0) ) {

				// get the mouse position within the game grid
				int x = 0, y = 0;
				input->getMouseXY(&x, &y);

				int action = -1;
				
				// for connect four
				if (m_gameType == CONNECTFOUR) {
					action = (x - 100) / 100;
				}
				// for tic-tac-toe
				else if (m_gameType == TICTACTOE) {
					if ((x - 100) / 100 < 3 &&
						(y - 100) / 100 < 3)
						action = ((y - 100) / 100) * 3 + (x - 100) / 100;
				}

				if (m_game->isActionValid(action))
					m_game->performAction(action);
			}
		}
		else {
			// it is the opponent's turn (player 2)
			// use the A.I. to make a decision
			m_game->performAction(m_ai->makeDecision(*m_game));
		}
	}
}

void GameTreesApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();
	
	// draw the game board
	m_game->draw(m_2dRenderer);

	// display if anyone has won
	m_2dRenderer->setRenderColour(1, 1, 0);
	auto result = m_game->getCurrentPlayState();
	if (result == GameTree::ePlayState::PLAYER_ONE)
		m_2dRenderer->drawText(m_font, "Player One is the winner!", 0, 0);
	else if (result == GameTree::ePlayState::PLAYER_TWO)
		m_2dRenderer->drawText(m_font, "Player Two is the winner!", 0, 0);
	else if (result == GameTree::ePlayState::DRAW)
		m_2dRenderer->drawText(m_font, "It's a draw!", 0, 0);

	char buf[256];

	// draw monte carlo scores
	if (m_aiType == MONTECARLO &&
		m_gameType == CONNECTFOUR) {
		for (auto& scores : ((MonteCarloAI*)m_ai)->m_scoresForActions) {

			sprintf(buf, "%.2f", scores.second / ((MonteCarloAI*)m_ai)->m_playouts);

			m_2dRenderer->drawText(m_font, buf, 100 + scores.first * 100, 50);
		}
	}

	// done drawing sprites
	m_2dRenderer->end();
}