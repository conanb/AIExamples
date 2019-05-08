#include "TicTacToeGame.h"
#include "Renderer2D.h"

bool TicTacToeGame::isActionValid(int action) const {
	return action >= 0 &&
		action < 9 &&
		m_board[action / 3][action % 3] == GameTree::ePlayState::UNKNOWN;
}

void TicTacToeGame::getValidActions(std::vector<int>& actions) const {
	actions.clear();
	for (int i = 0; i < 9; i++)
		if (isActionValid(i))
			actions.push_back(i);
}

GameTree::Game* TicTacToeGame::clone() const {
	TicTacToeGame* game = new TicTacToeGame();
	game->m_currentPlayer = m_currentPlayer;
	game->m_currentOpponent = m_currentOpponent;
	memcpy(game->m_board, m_board, sizeof(GameTree::ePlayState) * 9);
	return game;
}

void TicTacToeGame::performAction(int action) {

	// set the piece based on current player
	m_board[action / 3][action % 3] = m_currentPlayer;

	// switch players around
	m_currentPlayer = (m_currentPlayer == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
}

void TicTacToeGame::reverseAction(int action) {

	// set the piece based on current player
	m_board[action / 3][action % 3] = GameTree::ePlayState::UNKNOWN;

	// switch players around
	m_currentPlayer = (m_currentPlayer == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
}

void TicTacToeGame::draw(app::Renderer2D* renderer) const {

	// draw game board
	renderer->setRenderColour(1, 1, 1);

	for (int y = 0; y < 4; y++)
		renderer->drawLine(100, 100 + y * 100.f, 100 + 3 * 100.f, 100 + y * 100.f);

	for (int x = 0; x < 4; x++)
		renderer->drawLine(100 + x * 100.f, 100, 100 + x * 100.f, 100 + 3 * 100.f);

	// draw pieces
	for (int row = 0; row < 3; row++) {
		for (int col = 0; col < 3; col++) {

			float x = 150 + col * 100.f;
			float y = 150 + row * 100.f;

			if (m_board[row][col] == GameTree::ePlayState::PLAYER_ONE) {
				renderer->setRenderColour(0, 0, 1);
				renderer->drawCircle(x, y, 40);
			}
			else if (m_board[row][col] == GameTree::ePlayState::PLAYER_TWO) {
				renderer->setRenderColour(1, 0, 0);
				renderer->drawCircle(x, y, 40);
			}
		}
	}
}

GameTree::ePlayState TicTacToeGame::getCurrentPlayState() const {

	if (m_board[0][0] != GameTree::ePlayState::UNKNOWN &&
		((m_board[0][0] == m_board[0][1] &&
		  m_board[0][0] == m_board[0][2]) ||
		 (m_board[0][0] == m_board[1][0] &&
		  m_board[0][0] == m_board[2][0]) ||
		 (m_board[0][0] == m_board[1][1] &&
		  m_board[0][0] == m_board[2][2]))) {
		return m_board[0][0];
	}

	if (m_board[2][2] != GameTree::ePlayState::UNKNOWN &&
		((m_board[2][2] == m_board[2][1] &&
		  m_board[2][2] == m_board[2][0]) ||
		 (m_board[2][2] == m_board[1][2] &&
		  m_board[2][2] == m_board[0][2]))) {
		return m_board[2][2];
	}

	if (m_board[0][2] != GameTree::ePlayState::UNKNOWN &&
		(m_board[0][2] == m_board[1][1] &&
		 m_board[0][2] == m_board[2][0])) {
		return m_board[0][2];
	}

	if (m_board[1][1] != GameTree::ePlayState::UNKNOWN &&
		(m_board[1][1] == m_board[0][1] &&
		 m_board[1][1] == m_board[2][1])) {
		return m_board[1][1];
	}

	// NOTE: This was missing from Portal upload!
	if (m_board[1][1] != GameTree::ePlayState::UNKNOWN &&
		(m_board[1][1] == m_board[1][0] &&
		 m_board[1][1] == m_board[1][2])) {
		return m_board[1][1];
	}

	// check draw
	bool draw = true;
	for (int i = 0; i < 9; i++) {
		if (isActionValid(i)) {
			draw = false;
			break;
		}
	}
	if (draw)
		return GameTree::ePlayState::DRAW;

	// no winner
	return GameTree::ePlayState::UNKNOWN;
}