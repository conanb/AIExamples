#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "Pathfinding.h"
#include "Behaviour.h"
#include "Entity.h"
#include "Condition.h"

// a pathfinding node with a 2D position
class MyNode : public Pathfinding::Node {
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
class FollowPathBehaviour : public Behaviour {
public:

	FollowPathBehaviour() {}
	virtual ~FollowPathBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime);
};

class NewPathBehaviour : public Behaviour {
public:

	NewPathBehaviour(std::vector<MyNode*>& nodes) : m_nodes(nodes) {}
	virtual ~NewPathBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime);

protected:

	std::vector<MyNode*>& m_nodes;
};

class ValidPathCondition : public Condition {
public:

	ValidPathCondition() {}
	virtual ~ValidPathCondition() {}

	virtual bool test(Entity* entity) const {

		std::list<Pathfinding::Node*>* path = nullptr;
		if (entity->getBlackboard().get("path", &path)) {

			return path->empty() == false;
		}

		return false;
	}
};

class PathfindingApp : public Application {
public:

	PathfindingApp();
	virtual ~PathfindingApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	Renderer2D*	m_2dRenderer;
	Font*		m_font;

	Texture			m_map;
	std::vector<MyNode*>	m_nodes;
	
	Entity						m_player;
	//PathBehaviour					m_pathBehaviour;
	std::list<Pathfinding::Node*>	m_path;

	Texture	m_spriteSheet;
};