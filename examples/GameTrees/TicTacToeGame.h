#pragma once

#include "GameTreeBase.h"

class TicTacToeGame : public GameTree::Game {
public:

	TicTacToeGame() {
		memset(m_board, 0, sizeof(GameTree::ePlayState) * 9);
	}
	virtual ~TicTacToeGame() {}

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

	GameTree::ePlayState	m_board[3][3];
};