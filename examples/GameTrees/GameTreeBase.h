#pragma once

#include <vector>
#include <stdlib.h>

namespace app {
	class Renderer2D;
}

namespace GameTree {

// current state of a zero-sum game
enum ePlayState {
	UNKNOWN,
	PLAYER_ONE,
	PLAYER_TWO,
	DRAW,
};

// base class to define a zero-sum game with two opponents.
// actions are defined as integers
class Game {
public:

	Game() :
		m_currentPlayer(PLAYER_ONE),
		m_currentOpponent(PLAYER_TWO) {
	}
	virtual ~Game() {}

	// returns wether someone is winning or not
	virtual ePlayState	getCurrentPlayState() const = 0;

	// these switch whenever an action is made
	ePlayState		getCurrentPlayer() const	{ return m_currentPlayer;	}
	ePlayState		getCurrentOpponent() const	{ return m_currentOpponent; }

	// get a list of valid actions for current player
	virtual bool	isActionValid(int action) const = 0;
	virtual void	getValidActions(std::vector<int>& actions) const = 0;

	// performs an action for the current player, and switches current player
	virtual void	performAction(int action) = 0;
	virtual void	reverseAction(int action) = 0;

	// draw the game
	virtual void	draw(app::Renderer2D* renderer) const = 0;

	// clones the current game state
	virtual Game*	clone() const = 0;

protected:

	ePlayState	m_currentPlayer;
	ePlayState	m_currentOpponent;
};

// base class for an A.I. opponent
class AIPlayer {
public:

	AIPlayer() {}
	virtual ~AIPlayer() {}

	// makes a decision for the current game
	virtual int	makeDecision(const Game& game) = 0;
};

// an A.I. opponent that chooses purely random actions to perform
class RandomAI : public AIPlayer {
public:

	RandomAI() {}
	virtual ~RandomAI() {}

	// randomly choose an action to perform
	virtual int	makeDecision(const Game& game) {

		std::vector<int> actions;
		game.getValidActions(actions);

		if (actions.size() > 0)
			return actions[rand() % actions.size()];
		else
			return -1;
	}
};

}