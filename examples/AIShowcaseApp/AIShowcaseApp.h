#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "Timing.h"
#include "Pathfinding.h"
#include "Entity.h"
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
class MyNode : public graph::Node {
public:

	MyNode(float x, float y, float z = 0) : position(x,y,z) {}
	virtual ~MyNode() {}

	static float heuristic(Node* a, Node* b) {

		MyNode* s = (MyNode*)a;
		MyNode* e = (MyNode*)b;

		auto diff = e->position - s->position;

		return glm::dot(diff, diff);
	}
	
	glm::vec3 position;
};

class AttackTimerBehaviour : public ai::Behaviour {
public:

	AttackTimerBehaviour() {}
	virtual ~AttackTimerBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		// simply reduce the attack timer
		float timer = 0;
		if (entity->getBlackboard().get("attackTimer", timer)) {
			timer -= app::Time::deltaTime();
			entity->getBlackboard().set("attackTimer", timer);
		}
		return ai::eBehaviourResult::SUCCESS;
	}
};

class FollowPathBehaviour : public ai::Behaviour {
public:

	FollowPathBehaviour() {}
	virtual ~FollowPathBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity);
};

class NewPathBehaviour : public ai::Behaviour {
public:

	NewPathBehaviour(std::vector<MyNode*>& nodes) : m_nodes(nodes) {}
	virtual ~NewPathBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity);

protected:

	MyNode* findClosest(const glm::vec3& p);

	std::vector<MyNode*>& m_nodes;
};

class WaterAvoidanceForce : public ai::SteeringForce {
public:
	WaterAvoidanceForce() : m_map(nullptr), m_feelerLength(1) {}
	virtual ~WaterAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	void setMapData(app::Texture* map, float tileWidth, float tileHeight) {
		m_map = map;
		m_tileWidth = tileWidth;
		m_tileHeight = tileHeight;
	}

	// pure virtual function
	virtual glm::vec3 getForce(ai::Entity* entity) const;

protected:

	void collideWithBox(const glm::vec3& p, const glm::vec3& v,
						const glm::vec3& c, glm::vec3& force) const;

	app::Texture*	m_map;
	float			m_tileWidth, m_tileHeight;
	float			m_feelerLength;
};

class IsDeadBehaviour : public ai::Behaviour {
public:

	IsDeadBehaviour() {}
	virtual ~IsDeadBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		float health;
		if (entity->getBlackboard().get("health", health) &&
			health <= 0.f) {
			return ai::eBehaviourResult::SUCCESS;
		}
		return ai::eBehaviourResult::FAILURE;
	}
};

class RespawnBehaviour : public ai::Behaviour {
public:

	RespawnBehaviour() {}
	virtual ~RespawnBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		std::function<void(ai::Entity*)>* respawnFunction = nullptr;
		if (entity->getBlackboard().get("respawnFunction", &respawnFunction) &&
			respawnFunction != nullptr &&
			(*respawnFunction)) {
			(*respawnFunction)(entity);
		}
		return ai::eBehaviourResult::SUCCESS;
	}
};

class TrackClosestBehaviour : public ai::Behaviour {
public:

	TrackClosestBehaviour() {}
	virtual ~TrackClosestBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {

		std::vector<ai::Entity>* targets = nullptr;
		if (entity->getBlackboard().get("targets", &targets)) {

			ai::Entity* closest = nullptr;
			float closestDist = 99999.f * 99999.f;
			auto position = entity->getPosition();

			for (auto& t : *targets) {
				auto target = t.getPosition();
				auto diff = position - target;

				float dist = glm::dot(diff, diff);
				if (dist < closestDist) {
					closest = &t;
					closestDist = dist;
				}
			}

			entity->getBlackboard().set("target", closest);
		}
		return ai::eBehaviourResult::SUCCESS;
	}
};

class ClosestWithinAttackRangeBehaviour : public ai::Behaviour {
public:

	ClosestWithinAttackRangeBehaviour() {}
	virtual ~ClosestWithinAttackRangeBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		float attackRange = 0;
		ai::Entity* target = nullptr;
		if (entity->getBlackboard().get("attackRange", attackRange) &&
			entity->getBlackboard().get("target", &target) &&
			target != nullptr) {

			auto position = entity->getPosition();
			auto targetPos = target->getPosition();
			auto diff = position - targetPos;
			
			if (glm::dot(diff, diff) <= (attackRange * attackRange))
				return ai::eBehaviourResult::SUCCESS;
		}
		return ai::eBehaviourResult::FAILURE;
	}
};

class ClosestWithinChaseRangeBehaviour : public ai::Behaviour {
public:

	ClosestWithinChaseRangeBehaviour() {}
	virtual ~ClosestWithinChaseRangeBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		float chaseRange = 0;
		ai::Entity* target = nullptr;
		if (entity->getBlackboard().get("chaseRange", chaseRange) &&
			entity->getBlackboard().get("target", &target) &&
			target != nullptr) {

			auto position = entity->getPosition();
			auto targetPos = target->getPosition();
			auto diff = position - targetPos;

			if (glm::dot(diff, diff) <= (chaseRange * chaseRange))
				return ai::eBehaviourResult::SUCCESS;
		}
		return ai::eBehaviourResult::FAILURE;
	}
};

class AttackClosestBehaviour : public ai::Behaviour {
public:

	AttackClosestBehaviour() {}
	virtual ~AttackClosestBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		float attackTimer = 0;
		float attackSpeed = 0;
		ai::Entity* target = nullptr;
		float strength = 0;
		float health = 0;
		if (entity->getBlackboard().get("attackTimer", attackTimer) &&
			attackTimer <= 0.f &&
			entity->getBlackboard().get("attackSpeed", attackSpeed) &&
			entity->getBlackboard().get("target", &target) &&
			target != nullptr &&
			entity->getBlackboard().get("strength", strength) &&
			target->getBlackboard().get("health", health)) {
			health -= strength;
			target->getBlackboard().set("health", health);

			entity->getBlackboard().set("attackTimer", attackSpeed);
			return ai::eBehaviourResult::SUCCESS;
		}
		return ai::eBehaviourResult::FAILURE;
	}
};

class SeekClosestForce : public ai::SteeringForce {
public:

	SeekClosestForce() {}
	virtual ~SeekClosestForce() {}

	virtual glm::vec3 getForce(ai::Entity* entity) const;
};

class EnumBehaviour : public ai::CompositeBehaviour {
public:

	EnumBehaviour() {}
	virtual ~EnumBehaviour() {}

	virtual ai::eBehaviourResult execute(ai::Entity* entity) {
		int index = 0;
		entity->getBlackboard().get("type", index);

		return m_children[index]->execute(entity);
	}
};

class AIShowcaseApp : public app::Application {
public:

	AIShowcaseApp();
	virtual ~AIShowcaseApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	bool isTileWalkable(int x, int y) {
		return m_tiles[y * m_map->getWidth() + x] < eSprites::eSprite_UnwalkableStart;
	}

	struct Sprite {
		float u, v;
	};

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;
	app::Texture*	m_map;
	app::Texture*	m_background;
	app::Texture*	m_spriteSheet;
	app::Texture*	m_characterSprites;

	int*	m_tiles;
	Sprite	m_sprites[eSprites_Count];

	std::vector<MyNode*>	m_pathNodes;

	enum eEntityType {
		KNIGHT = 0,
		CAVEMAN,
	};

	std::vector<ai::Entity> m_knights;
	std::vector<ai::Entity> m_cavemen;

	AttackTimerBehaviour m_attackTimerBehaviour;

	ai::SelectorBehaviour m_knightBehaviour;
	FollowPathBehaviour	m_followPathBehaviour;
	NewPathBehaviour m_newPathBehaviour;

	ai::SteeringBehaviour m_cavemanBehaviour;
	WaterAvoidanceForce m_waterAvoidanceForce;
	ai::WanderForce m_wanderForce;

	ai::SteeringBehaviour m_chaseBehaviour;
	SeekClosestForce m_seekClosestForce;

	ai::SelectorBehaviour m_rootBehaviour;

	IsDeadBehaviour m_isDeadBehaviour;
	RespawnBehaviour m_respawnBehaviour;

	ai::SequenceBehaviour m_attackRootBehaviour;
	AttackClosestBehaviour m_attackBehaviour;

	ai::SequenceBehaviour m_chaseRootBehaviour;
	ai::SequenceBehaviour m_deathRootBehaviour;

	EnumBehaviour m_subBehaviour;

	ClosestWithinAttackRangeBehaviour m_withinAttackRangeBehaviour;
	ClosestWithinChaseRangeBehaviour m_withinChaseRangeBehaviour;

	TrackClosestBehaviour m_trackClosestBehaviour;

	int m_knightDeaths = 0;
	int m_cavemanDeaths = 0;

	// debug data
	bool m_drawGizmos = false;
};