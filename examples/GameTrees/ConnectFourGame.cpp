#include "ConnectFourGame.h"
#include "Renderer2D.h"

bool ConnectFourGame::isActionValid(int action) const {
	return action >= 0 &&
		action < COLUMNS &&
		m_piecesPerColumn[action] < ROWS;
}

void ConnectFourGame::getValidActions(std::vector<int>& actions) const {
	actions.clear();
	for (int i = 0; i < COLUMNS; i++)
		if (isActionValid(i))
			actions.push_back(i);
}

GameTree::Game* ConnectFourGame::clone() const {
	ConnectFourGame* game = new ConnectFourGame();
	game->m_currentPlayer = m_currentPlayer;
	game->m_currentOpponent = m_currentOpponent;
	game->m_bluePieces = m_bluePieces;
	game->m_redPieces = m_redPieces;
	for (int i = 0; i < COLUMNS; i++)
		game->m_piecesPerColumn[i] = m_piecesPerColumn[i];
	return game;
}

void ConnectFourGame::performAction(int action) {
	// get the location on the board
	long long pos = ((long long)1 << (m_piecesPerColumn[action] + 7 * action));

	// set the piece based on current player
	if (m_currentPlayer == GameTree::ePlayState::PLAYER_ONE)
		m_redPieces ^= pos;
	else
		m_bluePieces ^= pos;

	// increase the pieces in the column
	m_piecesPerColumn[action]++;

	// switch players around
	m_currentPlayer = (m_currentPlayer == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
}

void ConnectFourGame::reverseAction(int action) {
	// get the location on the board
	long long pos = ((long long)1 << (m_piecesPerColumn[action] + 7 * action));

	// set the piece based on current player
	if (m_currentPlayer == GameTree::ePlayState::PLAYER_ONE)
		m_redPieces ^= pos;
	else
		m_bluePieces ^= pos;

	// increase the pieces in the column
	m_piecesPerColumn[action]--;

	// switch players around
	m_currentPlayer = (m_currentPlayer == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
	m_currentOpponent = (m_currentOpponent == GameTree::ePlayState::PLAYER_TWO) ? GameTree::ePlayState::PLAYER_ONE : GameTree::ePlayState::PLAYER_TWO;
}

GameTree::ePlayState ConnectFourGame::getPieceAt(int r, int c) const {
	long long number = ((long long)1 << (r + 7 * c));

	bool test = (m_bluePieces & number) > 0;
	if (test)
		return GameTree::ePlayState::PLAYER_TWO;
	test = (m_redPieces & number) > 0;
	if (test)
		return GameTree::ePlayState::PLAYER_ONE;
	return GameTree::ePlayState::UNKNOWN;
}

void ConnectFourGame::draw(app::Renderer2D* renderer) const {

	// draw game board
	renderer->setRenderColour(1, 1, 1);

	for (int y = 0; y < ROWS+1; y++)
		renderer->drawLine(100, 100 + y * 100.f, 100 + COLUMNS * 100.f, 100 + y * 100.f);

	for (int x = 0; x < COLUMNS+1; x++)
		renderer->drawLine(100 + x * 100.f, 100, 100 + x * 100.f, 100 + ROWS * 100.f);

	// draw pieces
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLUMNS; col++) {			

			float x = 150 + col * 100.f;
			float y = 150 + row * 100.f;

			if (getPieceAt(row, col) == GameTree::ePlayState::PLAYER_ONE) {
				renderer->setRenderColour(0, 0, 1);
				renderer->drawCircle(x, y, 40);
			}
			else if (getPieceAt(row, col) == GameTree::ePlayState::PLAYER_TWO) {
				renderer->setRenderColour(1, 0, 0);
				renderer->drawCircle(x, y, 40);
			}
		}
	}
}

GameTree::ePlayState ConnectFourGame::getCurrentPlayState() const {

	// test player two
	long long y = m_bluePieces & (m_bluePieces >> 6);
	if ((y & (y >> 2 * 6)) > 0) // check \ diagonal
		return GameTree::ePlayState::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 7);
	if ((y & (y >> 2 * 7)) > 0) // check horizontal -
		return GameTree::ePlayState::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 8);
	if ((y & (y >> 2 * 8)) > 0) // check / diagonal
		return GameTree::ePlayState::PLAYER_TWO;
	y = m_bluePieces & (m_bluePieces >> 1);
	if ((y & (y >> 2)) > 0)     // check vertical |
		return GameTree::ePlayState::PLAYER_TWO;

	// test player one
	long long yy = m_redPieces & (m_redPieces >> 6);
	if ((yy & (yy >> 2 * 6)) > 0) // check \ diagonal
		return GameTree::ePlayState::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 7);
	if ((yy & (yy >> 2 * 7)) > 0) // check horizontal -
		return GameTree::ePlayState::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 8);
	if ((yy & (yy >> 2 * 8)) > 0) // check / diagonal
		return GameTree::ePlayState::PLAYER_ONE;
	yy = m_redPieces & (m_redPieces >> 1);
	if ((yy & (yy >> 2)) > 0)     // check vertical |
		return GameTree::ePlayState::PLAYER_ONE;

	// check draw
	bool draw = true;
	for (int col = 0; col < COLUMNS; col++) {
		if (isActionValid(col)) {
			draw = false;
			break;
		}
	}
	if (draw)
		return GameTree::ePlayState::DRAW;

	// no winner
	return GameTree::ePlayState::UNKNOWN;
}