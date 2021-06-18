#include "MiniMaxAI.h"

int MiniMaxAI::makeDecision(const GameTree::Game& game) {

	// get all potential actions
	std::vector<int> actions;
	game.getValidActions(actions);

	// set some variables to track best action
	int bestAction = -1;
	float bestScore = -99999;

	// score all actions to find best action
	for (auto action : actions) {
		float score = scoreAction(game, action, 1);

		if (score > bestScore) {
			bestScore = score;
			bestAction = action;
		}
	}

	return bestAction;
}

float MiniMaxAI::scoreAction(const GameTree::Game& game, int action, int depth) {

	auto gameClone = game.clone();
	gameClone->performAction(action);

	// check if game is over
	if (gameClone->getCurrentPlayState() != GameTree::ePlayState::UNKNOWN) {
		// game is over, but what was the result?
		if (gameClone->getCurrentPlayState() == GameTree::DRAW)
			return 0;

		// return + or - depending if we won or lost
		return (gameClone->getCurrentPlayState() == m_playerID) ? 10.0f : -10.0f;
	}

	if (depth >= MAX_DEPTH) {
		delete gameClone;
		return 0;
	}

	// expand the tree further down and get best / worst result
	std::vector<int> actions;
	gameClone->getValidActions(actions);

	std::vector<float> scores;

	for (auto a : actions) {

		// add child scores to the list
		scores.push_back(scoreAction(*gameClone, a, depth + 1));
	}

	float bestScore = 0;

	if (gameClone->getCurrentPlayer() == m_playerID) {
		// return highest score
		bestScore = *std::max_element(scores.begin(), scores.end());
	}
	else {
		// return the lowest score
		bestScore = *std::min_element(scores.begin(), scores.end());
	}

	delete gameClone;
	return bestScore;
}