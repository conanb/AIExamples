#pragma once

#include "GameTreeBase.h"

#include <map>

class MonteCarloAI : public GameTree::AIPlayer {
public:

	MonteCarloAI(int playouts) : m_playouts(playouts) {}
	virtual ~MonteCarloAI() {}

	virtual int makeDecision(const GameTree::Game& game);

	int getPlayouts() const { return m_playouts; }

	const std::map<int, float>& getScoresForEachAction() const { return m_scoresForActions; }

private:

	int m_playouts;

	std::map< int, float > m_scoresForActions;

	float expand(const GameTree::Game& game, int action);

	GameTree::ePlayState simulate(GameTree::Game* game);
};