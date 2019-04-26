#pragma once

#include "GameTreeBase.h"

#include <map>

class MonteCarloAI : public GameTree::AIPlayer {
public:

	int m_playouts;

	std::map< int, float > m_scoresForActions;

	MonteCarloAI(int playouts) : m_playouts(playouts) {}
	virtual ~MonteCarloAI() {}

	virtual int makeDecision(const GameTree::Game& game) {

		// get all actions we might perform
		std::vector<int> actions;
		game.getValidActions(actions);

		// early outs
		if (actions.empty())
			return -1;
		if (actions.size() == 1)
			return actions[0];

		// container for tracking score for each action
		for (auto& action : m_scoresForActions)
			action.second = 0;

		// do playouts for each action equally
		for (auto action : actions) {
			for (int i = 0; i < m_playouts; ++i) {
				// expand
				float score = expand(game, action);

				// accumulate backpropagated score
				m_scoresForActions[action] += score;
			}
		}

		// best action to perform will have highest score
		int bestAction = -1;
		float bestScore = -m_playouts;

		for (auto action : m_scoresForActions) {
			if (action.second > bestScore) {

				bestAction = action.first;
				bestScore = action.second;
			}
		}

		return bestAction;
	}

	float expand(const GameTree::Game& game, int action) {

		GameTree::Game* gameClone = game.clone();

		// expand current game based on selection
		gameClone->performAction(action);

		// simulate expanded choice
		auto winner = simulate(gameClone);

		delete gameClone;

		// backpropagate a score
		if (winner == game.getCurrentPlayer())
			return 1.0f;
		else if (winner == game.getCurrentOpponent())
			return -1.0f;
		else
			return 0.0f;
	}

	GameTree::ePlayState simulate(GameTree::Game* game) {

		std::vector<int> actions;

		// randomly make moves until the game ends
		while (game->getCurrentPlayState() == GameTree::ePlayState::UNKNOWN) {

			game->getValidActions(actions);

			int action = actions[rand() % actions.size()];

			game->performAction(action);
		}

		return game->getCurrentPlayState();
	}
};