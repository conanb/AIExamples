#pragma once

#include "GameTreeBase.h"

#include <algorithm>

class MiniMaxAI : public GameTree::AIPlayer {
public:

	MiniMaxAI(GameTree::ePlayState playerID) : m_playerID(playerID) {}
	virtual ~MiniMaxAI() {}

	virtual int makeDecision(const GameTree::Game& game);

protected:

	const int MAX_DEPTH = 7;

	GameTree::ePlayState m_playerID;

	float scoreAction(const GameTree::Game& game, int action, int depth);
};