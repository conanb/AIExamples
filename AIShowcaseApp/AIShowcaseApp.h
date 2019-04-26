#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Texture.h"

#include "Pathfinding.h"
#include "GameObject.h"
#include "Behaviour.h"
#include "BehaviourTree.h"
#include "SteeringBehaviour.h"

enum eSprites {
	SPRITE_GRASS = 0,

	// dirt
	eSprite_DirtStart,
	SPRITE_DIRT = eSprite_DirtStart,
	SPRITE_DIRTTL,
	SPRITE_DIRTT,
	SPRITE_DIRTTR,
	SPRITE_DIRTL,
	SPRITE_DIRTR,
	SPRITE_DIRTBL,
	SPRITE_DIRTB,
	SPRITE_DIRTBR,
	SPRITE_DIRTV,
	SPRITE_DIRTH,

	SPRITE_DIRTTT,
	SPRITE_DIRTBT,
	SPRITE_DIRTLT,
	SPRITE_DIRTRT,

	SPRITE_DIRTCORNER1,
	SPRITE_DIRTCORNER2,
	SPRITE_DIRTCORNER3,
	SPRITE_DIRTCORNER4,
	eSprite_DirtEnd = SPRITE_DIRTCORNER4,

	eSprite_UnwalkableStart,

	// water
	eSprite_WaterStart = eSprite_UnwalkableStart,
	SPRITE_WATER = eSprite_WaterStart,
	SPRITE_WATERTL,
	SPRITE_WATERT,
	SPRITE_WATERTR,
	SPRITE_WATERL,
	SPRITE_WATERR,
	SPRITE_WATERBL,
	SPRITE_WATERB,
	SPRITE_WATERBR,

	SPRITE_WATERCORNER1,
	SPRITE_WATERCORNER2,
	SPRITE_WATERCORNER3,
	SPRITE_WATERCORNER4,
	eSprite_WaterEnd = SPRITE_WATERCORNER4,

	SPRITE_TREE,

	eSprites_Count,
};

// a pathfinding node with a 2D position
class MyNode : public Pathfinding::Node {
public:

	MyNode(float x, float y) : x(x), y(y) {}
	virtual ~MyNode() {}

	static float heuristic(Node* a, Node* b) {

		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		float x = e->x - s->x;
		float y = e->y - s->y;

		return x * x + y * y;
	}
	
	float x, y;
};

class AttackTimerBehaviour : public Behaviour {
public:

	AttackTimerBehaviour() {}
	virtual ~AttackTimerBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		// simply reduce the attack timer
		float timer = 0;
		if (gameObject->getBlackboard().get("attackTimer", timer)) {
			timer -= deltaTime;
			gameObject->getBlackboard().set("attackTimer", timer);
		}
		return eBehaviourResult::SUCCESS;
	}
};

class FollowPathBehaviour : public Behaviour {
public:

	FollowPathBehaviour() {}
	virtual ~FollowPathBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);
};

class NewPathBehaviour : public Behaviour {
public:

	NewPathBehaviour(std::vector<MyNode*>& nodes) : m_nodes(nodes) {}
	virtual ~NewPathBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);

protected:

	MyNode* findClosest(float x, float y);

	std::vector<MyNode*>& m_nodes;
};

class WaterAvoidanceForce : public SteeringForce {
public:
	WaterAvoidanceForce() : m_map(nullptr), m_feelerLength(1) {}
	virtual ~WaterAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	void setMapData(aie::Texture* map, float tileWidth, float tileHeight) {
		m_map = map;
		m_tileWidth = tileWidth;
		m_tileHeight = tileHeight;
	}

	// pure virtual function
	virtual Force getForce(GameObject* gameObject) const;

protected:

	void collideWithBox(float x, float y, float vx, float vy,
						float boxX, float boxY, Force& force) const;

	aie::Texture*	m_map;
	float			m_tileWidth, m_tileHeight;
	float			m_feelerLength;
};

class IsDeadBehaviour : public Behaviour {
public:

	IsDeadBehaviour() {}
	virtual ~IsDeadBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		float health;
		if (gameObject->getBlackboard().get("health", health) &&
			health <= 0.f) {
			return eBehaviourResult::SUCCESS;
		}
		return eBehaviourResult::FAILURE;
	}
};

class RespawnBehaviour : public Behaviour {
public:

	RespawnBehaviour() {}
	virtual ~RespawnBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		std::function<void(GameObject*)>* respawnFunction = nullptr;
		if (gameObject->getBlackboard().get("respawnFunction", &respawnFunction) &&
			respawnFunction != nullptr &&
			(*respawnFunction)) {
			(*respawnFunction)(gameObject);
		}
		return eBehaviourResult::SUCCESS;
	}
};

class TrackClosestBehaviour : public Behaviour {
public:

	TrackClosestBehaviour() {}
	virtual ~TrackClosestBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {

		std::vector<GameObject>* targets = nullptr;
		if (gameObject->getBlackboard().get("targets", &targets)) {

			GameObject* closest = nullptr;
			float closestDist = 99999.f * 99999.f;
			float x, y, tx, ty;
			gameObject->getPosition(&x, &y);

			for (auto& t : *targets) {
				t.getPosition(&tx, &ty);

				float dist = (x - tx) * (x - tx) + (y - ty) * (y - ty);
				if (dist < closestDist) {
					closest = &t;
					closestDist = dist;
				}
			}

			gameObject->getBlackboard().set("target", closest);
		}
		return eBehaviourResult::SUCCESS;
	}
};

class ClosestWithinAttackRangeBehaviour : public Behaviour {
public:

	ClosestWithinAttackRangeBehaviour() {}
	virtual ~ClosestWithinAttackRangeBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		float attackRange = 0;
		GameObject* target = nullptr;
		if (gameObject->getBlackboard().get("attackRange", attackRange) &&
			gameObject->getBlackboard().get("target", &target) &&
			target != nullptr) {

			float x, y, tx, ty;
			gameObject->getPosition(&x, &y);
			target->getPosition(&tx, &ty);

			if (((x - tx) * (x - tx) + (y - ty) * (y - ty)) <= (attackRange * attackRange))
				return eBehaviourResult::SUCCESS;
		}
		return eBehaviourResult::FAILURE;
	}
};

class ClosestWithinChaseRangeBehaviour : public Behaviour {
public:

	ClosestWithinChaseRangeBehaviour() {}
	virtual ~ClosestWithinChaseRangeBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		float chaseRange = 0;
		GameObject* target = nullptr;
		if (gameObject->getBlackboard().get("chaseRange", chaseRange) &&
			gameObject->getBlackboard().get("target", &target) &&
			target != nullptr) {

			float x, y, tx, ty;
			gameObject->getPosition(&x, &y);
			target->getPosition(&tx, &ty);

			if (((x - tx) * (x - tx) + (y - ty) * (y - ty)) <= (chaseRange * chaseRange))
				return eBehaviourResult::SUCCESS;
		}
		return eBehaviourResult::FAILURE;
	}
};

class AttackClosestBehaviour : public Behaviour {
public:

	AttackClosestBehaviour() {}
	virtual ~AttackClosestBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		float attackTimer = 0;
		float attackSpeed = 0;
		GameObject* target = nullptr;
		float strength = 0;
		float health = 0;
		if (gameObject->getBlackboard().get("attackTimer", attackTimer) &&
			attackTimer <= 0.f &&
			gameObject->getBlackboard().get("attackSpeed", attackSpeed) &&
			gameObject->getBlackboard().get("target", &target) &&
			target != nullptr &&
			gameObject->getBlackboard().get("strength", strength) &&
			target->getBlackboard().get("health", health)) {
			health -= strength;
			target->getBlackboard().set("health", health);

			gameObject->getBlackboard().set("attackTimer", attackSpeed);
			return eBehaviourResult::SUCCESS;
		}
		return eBehaviourResult::FAILURE;
	}
};

class SeekClosestForce : public SteeringForce {
public:

	SeekClosestForce() {}
	virtual ~SeekClosestForce() {}

	virtual Force getForce(GameObject* gameObject) const;
};

class EnumBehaviour : public CompositeBehaviour {
public:

	EnumBehaviour() {}
	virtual ~EnumBehaviour() {}

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime) {
		int index = 0;
		gameObject->getBlackboard().get("type", index);

		return m_children[index]->execute(gameObject, deltaTime);
	}
};

class AIShowcaseApp : public Application {
public:

	AIShowcaseApp();
	virtual ~AIShowcaseApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

protected:

	bool isTileWalkable(int x, int y) {
		return m_tiles[y * m_map->getWidth() + x] < eSprites::eSprite_UnwalkableStart;
	}

	struct Sprite {
		float u, v;
	};

	Renderer2D*	m_2dRenderer;
	Font*		m_font;
	Texture*	m_map;
	Texture*	m_background;
	Texture*	m_spriteSheet;
	Texture*	m_characterSprites;

	int*	m_tiles;
	Sprite	m_sprites[eSprites_Count];

	std::vector<MyNode*>	m_pathNodes;

	enum eEntityType {
		KNIGHT = 0,
		CAVEMAN,
	};

	std::vector<GameObject> m_knights;
	std::vector<GameObject> m_cavemen;

	AttackTimerBehaviour m_attackTimerBehaviour;

	SelectorBehaviour m_knightBehaviour;
	FollowPathBehaviour	m_followPathBehaviour;
	NewPathBehaviour m_newPathBehaviour;

	SteeringBehaviour m_cavemanBehaviour;
	WaterAvoidanceForce m_waterAvoidanceForce;
	WanderForce m_wanderForce;

	SteeringBehaviour m_chaseBehaviour;
	SeekClosestForce m_seekClosestForce;

	SelectorBehaviour m_rootBehaviour;

	IsDeadBehaviour m_isDeadBehaviour;
	RespawnBehaviour m_respawnBehaviour;

	SequenceBehaviour m_attackRootBehaviour;
	AttackClosestBehaviour m_attackBehaviour;

	SequenceBehaviour m_chaseRootBehaviour;
	SequenceBehaviour m_deathRootBehaviour;

	EnumBehaviour m_subBehaviour;

	ClosestWithinAttackRangeBehaviour m_withinAttackRangeBehaviour;
	ClosestWithinChaseRangeBehaviour m_withinChaseRangeBehaviour;

	TrackClosestBehaviour m_trackClosestBehaviour;

	int m_knightDeaths = 0;
	int m_cavemanDeaths = 0;

	// debug data
	bool m_drawGizmos = false;
};