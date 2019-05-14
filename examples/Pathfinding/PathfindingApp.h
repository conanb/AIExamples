#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "Pathfinding.h"
#include "Behaviour.h"
#include "Agent.h"
#include "Condition.h"

// a pathfinding node with a 2D position
class MyNode : public graph::Node {
public:

	MyNode() {}
	virtual ~MyNode() {}

	static float heuristicManhattan(Node* a, Node* b) {

		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		return (e->x - s->x) + (e->y - s->y);
	}

	static float heuristicDistanceSqr(Node* a, Node* b) {

		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		float x = e->x - s->x;
		float y = e->y - s->y;

		return x * x + y * y;
	}

	static float heuristicDistance(Node* a, Node* b) {

		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		float x = e->x - s->x;
		float y = e->y - s->y;

		return sqrt(x * x + y * y);
	}
	
	float x, y;
};

// a behaviour that follows a path, then randomly picks
// a new target node when it reaches the end of the path
class FollowPathBehaviour : public ai::Behaviour {
public:

	FollowPathBehaviour() {}
	virtual ~FollowPathBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Agent* entity);
};

class NewPathBehaviour : public ai::Behaviour {
public:

	NewPathBehaviour(std::vector<MyNode*>& nodes) : m_nodes(nodes) {}
	virtual ~NewPathBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Agent* entity);

protected:

	std::vector<MyNode*>& m_nodes;
};

class ValidPathCondition : public ai::Condition {
public:

	ValidPathCondition() {}
	virtual ~ValidPathCondition() {}

	virtual bool test(ai::Agent* entity) const {

		std::list<graph::Node*>* path = nullptr;
		if (entity->getBlackboard().get("path", &path)) {

			return path->empty() == false;
		}

		return false;
	}
};

class PathfindingApp : public app::Application {
public:

	PathfindingApp();
	virtual ~PathfindingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	enum NodeFlags : unsigned int {
		MEDKIT = (1<<0)
	};

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	app::Texture			m_map;
	std::vector<MyNode*>	m_nodes;
	
	ai::Agent						m_player;
	//PathBehaviour					m_pathBehaviour;
	std::list<graph::Node*>	m_path;

	app::Texture	m_spriteSheet;
};