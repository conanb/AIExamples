#pragma once

#include "GameTreeBase.h"

class ConnectFourGame : public GameTree::Game {
public:

	enum {
		COLUMNS = 7,
		ROWS = 6,
	};

	ConnectFourGame() :
		m_bluePieces(0),
		m_redPieces(0) {
		for (int& i : m_piecesPerColumn)
			i = 0;
	}
	virtual ~ConnectFourGame() {}

	// returns wether someone is winning or not
	virtual GameTree::ePlayState getCurrentPlayState() const;

	// get a list of valid actions for current player
	virtual bool isActionValid(int action) const;
	virtual void getValidActions(std::vector<int>& actions) const;

	// performs an action for the current player, and switches current player
	virtual void performAction(int action);
	virtual void reverseAction(int action);

	// draw the game
	virtual void draw(app::Renderer2D* renderer) const;

	// clones the current game state
	virtual Game* clone() const;

private:

	GameTree::ePlayState getPieceAt(int r, int c) const;

	long long	m_bluePieces;
	long long	m_redPieces;

	int			m_piecesPerColumn[COLUMNS];
};