#include "AIShowcaseApp.h"
#include "Font.h"
#include "Input.h"
#include "Intersection.h"
#include <gl_core_4_4.h>

int getWaterSprite(unsigned char spriteId);
int getDirtSprite(unsigned char spriteId);
unsigned char getSpriteId(int x, int y, const unsigned char* pixels, unsigned int colour, int w, int h);

unsigned int getPixelColour3(int index, const unsigned char* pixels) {
	unsigned int c = 0;
	c |= pixels[index+2] << 0;
	c |= pixels[index+1] << 8;
	c |= pixels[index] << 16;
	return c;
}
unsigned int getPixelColour4(int index, const unsigned char* pixels) {
	unsigned int c = 0;
	c |= pixels[index + 3] << 0;
	c |= pixels[index + 2] << 8;
	c |= pixels[index + 1] << 16;
	c |= pixels[index] << 24;
	return c;
}

AIShowcaseApp::AIShowcaseApp() 
	: m_newPathBehaviour(m_pathNodes) {

}

AIShowcaseApp::~AIShowcaseApp() {

}

bool AIShowcaseApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("../../bin/font/consolas.ttf", 32);

	m_map = new app::Texture("../../bin/map/map.bmp");
	m_background = new app::Texture("../../bin/textures/background.png");
	m_spriteSheet = new app::Texture("../../bin/textures/roguelikeSheet_transparent.png");
	m_characterSprites = new app::Texture("../../bin/textures/roguelikeChar_transparent.png");

	// 1-pixel padding between sprites
	float pw = 1.0f / m_spriteSheet->getWidth();
	float ph = 1.0f / m_spriteSheet->getHeight();

	// size of sprites (16 pixels)
	float w = pw * 16;
	float h = ph * 16;

	// allocate map sprite locations
	m_sprites[SPRITE_GRASS] = { 5, 0 };
	m_sprites[SPRITE_TREE] = { 18, 9 };
	m_sprites[SPRITE_WATER] = { 3, 1 };
	m_sprites[SPRITE_WATERTL] = { 4, 0 };
	m_sprites[SPRITE_WATERT] = { 3, 0 };
	m_sprites[SPRITE_WATERTR] = { 2, 0 };
	m_sprites[SPRITE_WATERL] = { 4, 1 };
	m_sprites[SPRITE_WATERR] = { 2, 1 };
	m_sprites[SPRITE_WATERBL] = { 4, 2 };
	m_sprites[SPRITE_WATERB] = { 3, 2 };
	m_sprites[SPRITE_WATERBR] = { 2, 2 };
	m_sprites[SPRITE_WATERCORNER1] = { 1, 2 };
	m_sprites[SPRITE_WATERCORNER2] = { 0, 2 };
	m_sprites[SPRITE_WATERCORNER3] = { 1, 1 };
	m_sprites[SPRITE_WATERCORNER4] = { 0, 1 };
	m_sprites[SPRITE_DIRT] = { 6, 0 };
	m_sprites[SPRITE_DIRTTL] = { 9, 9 };
	m_sprites[SPRITE_DIRTT] = { 8, 9 };
	m_sprites[SPRITE_DIRTTR] = { 7, 9 };
	m_sprites[SPRITE_DIRTL] = { 9, 10 };
	m_sprites[SPRITE_DIRTR] = { 7, 10 };
	m_sprites[SPRITE_DIRTBL] = { 9, 11 };
	m_sprites[SPRITE_DIRTB] = { 8, 11 };
	m_sprites[SPRITE_DIRTBR] = { 7, 11 };
	m_sprites[SPRITE_DIRTV] = { 9, 7 };
	m_sprites[SPRITE_DIRTH] = { 9, 8 };
	m_sprites[SPRITE_DIRTCORNER1] = { 7, 11 };
	m_sprites[SPRITE_DIRTCORNER2] = { 9, 11 };
	m_sprites[SPRITE_DIRTCORNER3] = { 7, 9 };
	m_sprites[SPRITE_DIRTCORNER4] = { 9, 9 };
	m_sprites[SPRITE_DIRTTT] = { 6, 8 };
	m_sprites[SPRITE_DIRTBT] = { 5, 7 };
	m_sprites[SPRITE_DIRTLT] = { 6, 7 };
	m_sprites[SPRITE_DIRTRT] = { 5, 8 };

	m_tiles = new int[m_map->getWidth() * m_map->getHeight()];

	// set tiles and pathfinding graph
	auto pixels = m_map->getPixels();
	for (unsigned int y = 0, index = 0; y < m_map->getHeight(); ++y) {
		for (unsigned int x = 0; x < m_map->getWidth(); ++x, ++index) {

			m_tiles[index] = -1;

			auto pixelIndex = index * 3;

			// determine if it's a water tile
			if (getPixelColour3(pixelIndex,pixels) == 0x000000ff)
				m_tiles[index] = getWaterSprite(getSpriteId(x, y,
														  pixels, 0x000000ff,
														  m_map->getWidth(), m_map->getHeight()));
			// determing if it's a dirt tile
			if (getPixelColour3(pixelIndex, pixels) == 0x00603913) {
				m_tiles[index] = getDirtSprite(getSpriteId(x, y,
														  pixels, 0x00603913,
														  m_map->getWidth(), m_map->getHeight()));

				m_pathNodes.push_back(new MyNode(8 + x * 16.0f, 8 + y * 16.0f));
			}

			// add trees to grass tiles
			if (getPixelColour3(pixelIndex, pixels) == 0x00ffffff &&
				rand() % 50 == 0)
				m_tiles[index] = SPRITE_TREE;
		}
	}

	// create edges
	for (auto a : m_pathNodes) {
		for (auto b : m_pathNodes) {
			if (a == b) continue;

			auto diff = b->position - a->position;
			float sqrDist = glm::dot(diff, diff);

			if (sqrDist <= (30 * 30)) {
				graph::Edge* edge = new graph::Edge();
				edge->cost = sqrDist;
				edge->target = b;

				a->edges.push_back(edge);
			}
		}
	}

	// root
	m_rootBehaviour.addChild(&m_deathRootBehaviour);
	m_rootBehaviour.addChild(&m_attackRootBehaviour);
	m_rootBehaviour.addChild(&m_chaseRootBehaviour);
	m_rootBehaviour.addChild(&m_subBehaviour);

	// death
	m_deathRootBehaviour.addChild(&m_isDeadBehaviour);
	m_deathRootBehaviour.addChild(&m_respawnBehaviour);

	// attack
	m_attackRootBehaviour.addChild(&m_withinAttackRangeBehaviour);
	m_attackRootBehaviour.addChild(&m_attackBehaviour);

	// chase
	m_chaseRootBehaviour.addChild(&m_withinChaseRangeBehaviour);
	m_chaseRootBehaviour.addChild(&m_chaseBehaviour);

	m_chaseBehaviour.addForce(&m_waterAvoidanceForce);
	m_chaseBehaviour.addForce(&m_seekClosestForce);

	// sub
	m_subBehaviour.addChild(&m_knightBehaviour);
	m_subBehaviour.addChild(&m_cavemanBehaviour);

	// knight-sub
	m_knightBehaviour.addChild(&m_followPathBehaviour);
	m_knightBehaviour.addChild(&m_newPathBehaviour);

	// caveman-sub
	m_cavemanBehaviour.addForce(&m_wanderForce);
	m_cavemanBehaviour.addForce(&m_waterAvoidanceForce);

	// forces
	m_waterAvoidanceForce.setFeelerLength(30);
	m_waterAvoidanceForce.setMapData(m_map, 16, 16);

	// create some knights
	m_knights.resize(4);
	for (auto& knight : m_knights) {

		auto node = m_pathNodes[rand() % m_pathNodes.size()];

		knight.setPosition(node->position);

		knight.getBlackboard().set("path", new std::list<graph::Node*>(), true);
		knight.getBlackboard().set("velocity", new glm::vec3(0), true);
		knight.getBlackboard().set("wanderData", new ai::WanderData({ 100,75,25,{0,0,0},{1,1,0} }), true);
		knight.getBlackboard().set("maxForce", 100.f);
		knight.getBlackboard().set("maxVelocity", 40.f);
		knight.getBlackboard().set("speed", 50.0f);
		knight.getBlackboard().set("maxHealth", 100.f);
		knight.getBlackboard().set("health", 100.f);
		knight.getBlackboard().set("attackSpeed", 0.75f);
		knight.getBlackboard().set("attackTimer", 0.f);
		knight.getBlackboard().set("strength", 25.0f);
		knight.getBlackboard().set("attackRange", 25.0f);
		knight.getBlackboard().set("chaseRange", 50.0f);
		knight.getBlackboard().set("type", KNIGHT);
		knight.getBlackboard().set("targets", &m_cavemen);

		// respawn function uses a func ptr to a lambda
		auto myFunc = new std::function<void(ai::Entity*)>();

		*myFunc = [this](ai::Entity* entity) {
			entity->getBlackboard().set("health", 100.f);
			auto n = m_pathNodes[rand() % m_pathNodes.size()];
			entity->setPosition(n->position);

			std::list<graph::Node*>* path = nullptr;
			if (entity->getBlackboard().get("path", &path))
				path->clear();
			glm::vec3* velocity = nullptr;
			if (entity->getBlackboard().get("velocity", &velocity)) {
				velocity->x = velocity->y = velocity->z = 0;
			}

			++m_knightDeaths;
		};

		knight.getBlackboard().set("respawnFunction", myFunc, true);

		knight.addBehaviour(&m_attackTimerBehaviour);
		knight.addBehaviour(&m_trackClosestBehaviour);
		knight.addBehaviour(&m_rootBehaviour);
	}

	// create some cavemen
	m_cavemen.resize(12);
	for (auto& caveman : m_cavemen) {
		int index;
		do {
			index = rand() % (m_map->getWidth() * m_map->getHeight());
			caveman.setPosition({ 8 + 16.0f * (index % m_map->getWidth()), 8 + 16.0f * (index / m_map->getWidth()),0 });
		} while (m_tiles[index] != -1);

		caveman.getBlackboard().set("velocity", new glm::vec3(0), true);
		caveman.getBlackboard().set("wanderData", new ai::WanderData({ 100,75,25,{0,0,0},{1,1,0} }), true);
		caveman.getBlackboard().set("maxForce", 100.f);
		caveman.getBlackboard().set("maxVelocity", 40.f);
		caveman.getBlackboard().set("maxHealth", 100.f);
		caveman.getBlackboard().set("health", 100.f);
		caveman.getBlackboard().set("attackSpeed", 0.25f);
		caveman.getBlackboard().set("attackTimer", 0.f);
		caveman.getBlackboard().set("strength", 5.0f);
		caveman.getBlackboard().set("attackRange", 25.0f);
		caveman.getBlackboard().set("chaseRange", 100.0f);
		caveman.getBlackboard().set("type", CAVEMAN);
		caveman.getBlackboard().set("targets", &m_knights);

		caveman.getBlackboard().set("respawnFunction", new std::function<void(ai::Entity*)>(
			[this](ai::Entity* entity) {
			entity->getBlackboard().set("health", 100.f);
			
			int i;
			do {
				i = rand() % (m_map->getWidth() * m_map->getHeight());
				entity->setPosition({ 8 + 16.0f * (i % m_map->getWidth()), 8 + 16.0f * (i / m_map->getWidth()),0 });
			} while (m_tiles[i] != -1);
			glm::vec3* velocity = nullptr;
			if (entity->getBlackboard().get("velocity", &velocity)) {
				velocity->x = velocity->y = velocity->z = 0;
			}

			++m_cavemanDeaths;

		}), true);

		caveman.addBehaviour(&m_attackTimerBehaviour);
		caveman.addBehaviour(&m_trackClosestBehaviour);
		caveman.addBehaviour(&m_rootBehaviour);
	}

	return true;
}

void AIShowcaseApp::shutdown() {

	for (auto n : m_pathNodes)
		delete n;

	delete[] m_tiles;
	delete m_map;
	delete m_background;
	delete m_spriteSheet;
	delete m_characterSprites;
	delete m_font;
	delete m_2dRenderer;
}

void AIShowcaseApp::update() {

	for (auto& knight : m_knights)
		knight.executeBehaviours();
	for (auto& caveman : m_cavemen)
		caveman.executeBehaviours();
		
	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	// toggle debug rendering
	if (input->wasKeyPressed(app::INPUT_KEY_D))
		m_drawGizmos = !m_drawGizmos;
}

void AIShowcaseApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draw level
	float pw = 1.0f / m_spriteSheet->getWidth();
	float ph = 1.0f / m_spriteSheet->getHeight();
	float w = pw * 16.0f;
	float h = ph * 16.0f;

	// draw the sprites
	m_2dRenderer->setRenderColour(1, 1, 1);
	for (unsigned int y = 0, index = 0; y < m_map->getHeight(); ++y) {
		for (unsigned int x = 0; x < m_map->getWidth(); ++x, ++index) {
			
			if (m_tiles[y * 80 + x] != -1) {
				m_2dRenderer->setUVRect(
					m_sprites[m_tiles[index]].u * w + m_sprites[m_tiles[index]].u * pw,
					m_sprites[m_tiles[index]].v * h + m_sprites[m_tiles[index]].v * ph,
					w, h);
				m_2dRenderer->drawSprite(m_spriteSheet, x * 16.0f, y * 16.0f, 16, 16, 0, 0, 0, 0);
			}
		}
	}

	// draw pathfinding graph
	if (m_drawGizmos) {
		m_2dRenderer->setRenderColour(1, 0, 0);
		for (auto node : m_pathNodes) {
			m_2dRenderer->drawBox(node->position.x, node->position.y, 4, 4);

			for (auto edge : node->edges) {
				MyNode* target = (MyNode*)edge->target;

				m_2dRenderer->drawLine(node->position.x, node->position.y, target->position.x, target->position.y);
			}
		}
	}

	// draw knights and cavemen
	pw = 1.0f / m_characterSprites->getWidth();
	ph = 1.0f / m_characterSprites->getHeight();
	w = pw * 16.0f;
	h = ph * 16.0f;

	float health, maxHealth;

	ai::Entity* target = nullptr;

	m_2dRenderer->setUVRect(0, 1 - h, w, h);
	for (auto& knight : m_knights) {
		auto position = knight.getPosition();
		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawSprite(m_characterSprites, position.x, position.y, 16, 16);

		// draw health
		knight.getBlackboard().get("health", health);
		knight.getBlackboard().get("maxHealth", maxHealth);

		m_2dRenderer->setRenderColour(0, 1, 0);
		m_2dRenderer->drawBox(position.x, position.y + 12, 16 * (health / maxHealth), 2);
	}

	glm::vec3* v = nullptr;
	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	float s2 = sinf(3.14159f*-0.15f);
	float c2 = cosf(3.14159f*-0.15f);

	m_2dRenderer->setUVRect(0, h * 6 + ph * 6, w, h);
	for (auto& caveman : m_cavemen) {
		auto position = caveman.getPosition();
		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawSprite(m_characterSprites, position.x, position.y, 16, 16);

		// draw health
		caveman.getBlackboard().get("health", health);
		caveman.getBlackboard().get("maxHealth", maxHealth);

		m_2dRenderer->setRenderColour(0, 1, 0);
		m_2dRenderer->drawBox(position.x, position.y + 12, 16 * (health / maxHealth), 2);

		// draw feelers
		if (m_drawGizmos) {
			m_2dRenderer->setRenderColour(1, 1, 0);
			if (caveman.getBlackboard().get("velocity", &v)) {

				float vx = v->x;
				float vy = v->y;

				float magSqr = vx * vx + vy * vy;
				if (magSqr > 0) {
					magSqr = sqrt(magSqr);
					vx /= magSqr;
					vy /= magSqr;

					m_2dRenderer->drawLine(position.x, position.y, position.x + vx * 40, position.y + vy * 40);

					m_2dRenderer->drawLine(position.x, position.y, position.x + (vx * c - vy * s) * 40 * 0.5f, position.y + (vx * s + vy * c) * 40 * 0.5f);
					m_2dRenderer->drawLine(position.x, position.y, position.x + (vx * c2 - vy * s2) * 40 * 0.5f, position.y + (vx * s2 + vy * c2) * 40 * 0.5f);
				}
			}
		}
	}

	// render background into blank locations
	m_2dRenderer->setUVRect(0, 0, 1, 1);
	m_2dRenderer->setRenderColour(1, 1, 1);
	m_2dRenderer->drawSprite(m_background, 0, 0, 1280, 720, 0, 1, 0, 0);

	// list scores
	m_2dRenderer->setRenderColour(1, 1, 0);
	char buffer[256];
	sprintf(buffer, "Cavemen: %i", m_knightDeaths);
	m_2dRenderer->drawText(m_font, buffer, 0, 0, -1);
	sprintf(buffer, "Knights: %i", m_cavemanDeaths);
	m_2dRenderer->drawText(m_font, buffer, 0, 32, -1);
	m_2dRenderer->drawText(m_font, "__________", 0, 64, -1);
	m_2dRenderer->drawText(m_font, "Scores", 0, 72, -1);

	// done drawing sprites
	m_2dRenderer->end();
}

// determines a sprite index based on the 256 different
// configurations that could exist for a 3x3 matrix of pixels
unsigned char getSpriteId(int x, int y, const unsigned char* pixels, unsigned int colour, int w, int h) {

	auto indextl = y > 0 && x > 0 ? ((y - 1) * w + (x - 1)) * 3 : -1;
	auto indext = y > 0 ? ((y - 1) * w + x) * 3 : -1;
	auto indextr = y > 0 && x < (w - 1) ? ((y - 1) * w + (x + 1)) * 3 : -1;
	auto indexl = x > 0 ? (y * w + (x - 1)) * 3 : -1;
	auto indexr = x < (w - 1) ? (y * w + (x + 1)) * 3 : -1;
	auto indexbl = y < (h - 1) && x > 0 ? ((y + 1) * w + (x - 1)) * 3 : -1;
	auto indexb = y < (h - 1) ? ((y + 1) * w + x) * 3 : -1;
	auto indexbr = y < (h - 1) && x < (w - 1) ? ((y + 1) * w + (x + 1)) * 3 : -1;

	unsigned char index = 0;

	if (indextl == -1 ||
		getPixelColour3(indextl, pixels) == colour)
		index |= 1 << 0;
	if (indext == -1 ||
		getPixelColour3(indext, pixels) == colour)
		index |= 1 << 1;
	if (indextr == -1 ||
		getPixelColour3(indextr, pixels) == colour)
		index |= 1 << 2;
	if (indexl == -1 ||
		getPixelColour3(indexl, pixels) == colour)
		index |= 1 << 3;
	if (indexr == -1 ||
		getPixelColour3(indexr, pixels) == colour)
		index |= 1 << 4;
	if (indexbl == -1 ||
		getPixelColour3(indexbl, pixels) == colour)
		index |= 1 << 5;
	if (indexb == -1 ||
		getPixelColour3(indexb, pixels) == colour)
		index |= 1 << 6;
	if (indexbr == -1 ||
		getPixelColour3(indexbr, pixels) == colour)
		index |= 1 << 7;

	return index;
}

int getWaterSprite(unsigned char spriteId) {
	
	switch (spriteId) {
	case 31:	return SPRITE_WATERT;
	case 159:	return SPRITE_WATERT;
	case 63:	return SPRITE_WATERT;
	case 249:	return SPRITE_WATERB;
	case 252:	return SPRITE_WATERB;
	case 248:	return SPRITE_WATERB;
	case 107:	return SPRITE_WATERL;
	case 235:	return SPRITE_WATERL;
	case 111:	return SPRITE_WATERL;
	case 214:	return SPRITE_WATERR;
	case 246:	return SPRITE_WATERR;
	case 215:	return SPRITE_WATERR;
	case 11:	return SPRITE_WATERTL;
	case 22:	return SPRITE_WATERTR;	
	case 104:	return SPRITE_WATERBL;
	case 208:	return SPRITE_WATERBR;
	case 15:	return SPRITE_WATERTL;
	case 23:	return SPRITE_WATERTR;
	case 223:	return SPRITE_WATERCORNER1;
	case 127:	return SPRITE_WATERCORNER2;
	case 254:	return SPRITE_WATERCORNER3;
	case 251:	return SPRITE_WATERCORNER4;
	default:	return SPRITE_WATER;
	}
}

int getDirtSprite(unsigned char spriteId) {

	switch (spriteId) {
	case 31:	return SPRITE_DIRTT;
	case 159:	return SPRITE_DIRTT;
	case 63:	return SPRITE_DIRTT;
	case 248:	return SPRITE_DIRTB;
	case 249:	return SPRITE_DIRTB;
	case 252:	return SPRITE_DIRTB;
	case 107:	return SPRITE_DIRTL;
	case 235:	return SPRITE_DIRTL;
	case 111:	return SPRITE_DIRTL;
	case 246:	return SPRITE_DIRTR;
	case 215:	return SPRITE_DIRTR;
	case 214:	return SPRITE_DIRTR;
	case 11:	return SPRITE_DIRTTL;
	case 43:	return SPRITE_DIRTTL;
	case 22:	return SPRITE_DIRTTR;
	case 150:	return SPRITE_DIRTTR;
	case 104:	return SPRITE_DIRTBL;
	case 208:	return SPRITE_DIRTBR;
	case 80:	return SPRITE_DIRTCORNER1;
	case 212:	return SPRITE_DIRTCORNER1;
	case 105:	return SPRITE_DIRTCORNER2;
	case 72:	return SPRITE_DIRTCORNER2;
	case 18:	return SPRITE_DIRTCORNER3;
	case 10:	return SPRITE_DIRTCORNER4;
	case 98:	return SPRITE_DIRTV;
	case 194:	return SPRITE_DIRTV;
	case 70:	return SPRITE_DIRTV;
	case 67:	return SPRITE_DIRTV;
	case 66:	return SPRITE_DIRTV;
	case 71:	return SPRITE_DIRTV;
	case 226:	return SPRITE_DIRTV;
	case 24:	return SPRITE_DIRTH;
	case 56:	return SPRITE_DIRTH;
	case 152:	return SPRITE_DIRTH;
	case 28:	return SPRITE_DIRTH;
	case 25:	return SPRITE_DIRTH;
	case 57:	return SPRITE_DIRTH;
	case 156:	return SPRITE_DIRTH;
	case 88:	return SPRITE_DIRTTT;
	case 26:	return SPRITE_DIRTBT;
	case 74:	return SPRITE_DIRTRT;
	case 82:	return SPRITE_DIRTLT;
	default:	return SPRITE_DIRT;
	}
}

ai::eBehaviourResult FollowPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<graph::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false ||
		path->empty())
		return ai::eBehaviourResult::FAILURE;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	auto position = entity->getPosition();

	// access first node we're heading towards
	MyNode* first = (MyNode*)path->front();

	// distance to first
	auto diff = first->position - position;
	
	// if not at the target then move towards it
	if (glm::dot(diff, diff) > 25) {

		// move to target (can overshoot!)
		entity->translate(glm::normalize(diff) * speed * app::Time::deltaTime());
	}
	else {
		// at the node, remove it and move to the next
		path->pop_front();
	}
	return ai::eBehaviourResult::SUCCESS;
}

MyNode* NewPathBehaviour::findClosest(const glm::vec3& p) {

	MyNode* closest = nullptr;
	float closestDist = FLT_MAX;

	for (auto node : m_nodes) {

		auto diff = node->position - p;
		float dist = glm::dot(diff, diff);

		if (dist < closestDist) {
			closest = node;
			closestDist = dist;
		}
	}

	return closest;
}

ai::eBehaviourResult NewPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<graph::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false)
		return ai::eBehaviourResult::FAILURE;

	auto position = entity->getPosition();

	// random end node
	bool found = false;
	do {

		auto start = findClosest(position);
		auto end = m_nodes[rand() % m_nodes.size()];

		found = graph::Search::aStar(start, end, *path, MyNode::heuristic);

	} while (found == false);

	return ai::eBehaviourResult::SUCCESS;
}

glm::vec3 WaterAvoidanceForce::getForce(ai::Entity* entity) const {

	glm::vec3 force = { 0, 0, 0 };

	auto position = entity->getPosition();

	glm::vec3* velocity = nullptr;
	if (entity->getBlackboard().get("velocity", &velocity) == false)
		return force;
		
	// are we moving?
	float mag = glm::dot(*velocity, *velocity);
	if (mag > 0) {

		mag = sqrt(mag);

		auto vel = *velocity / mag * m_feelerLength;

		// find which cell we're in
		unsigned int cellX = unsigned int(position.x / m_tileWidth);
		unsigned int cellY = unsigned int(position.y / m_tileHeight);

		const unsigned char* pixels = m_map->getPixels();

		// test the 8 boxes
		if (cellX > 0 &&
			getPixelColour3(3 * (cellY * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{(cellX - 1) * m_tileWidth,
						   cellY * m_tileHeight,0 },
						   force);
		}
		if (cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{ cellX * m_tileWidth,
						   (cellY - 1) * m_tileHeight,0 },
						   force);
		}
		if (cellX > 0 &&
			cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{ (cellX - 1) * m_tileWidth,
						   (cellY - 1) * m_tileHeight,0 },
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			getPixelColour3(3 * (cellY * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{  (cellX + 1) * m_tileWidth,
						   cellY * m_tileHeight,0 },
						   force);
		}
		if (cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{  cellX * m_tileWidth,
						   (cellY + 1) * m_tileHeight,0 },
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{   (cellX - 1) * m_tileWidth,
						   (cellY + 1) * m_tileHeight,0 },
						   force);
		}
		if (cellX > 0 &&
			cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{  (cellX - 1) * m_tileWidth,
						   (cellY + 1) * m_tileHeight,0 },
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(position, vel,
				{ (cellX + 1) * m_tileWidth,
						   (cellY - 1) * m_tileHeight, 0 },
						   force);
		}
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return force * maxForce;
}

void WaterAvoidanceForce::collideWithBox(const glm::vec3& p, const glm::vec3& v,
										const glm::vec3& center,
										 glm::vec3& force) const {

	glm::vec3 n(0);
	if (intersection::rayBoxIntersection(p,
						   v,
		center, { m_tileWidth, m_tileHeight,0 },
						   n)) {
		force += n;
	}

	// rotate feeler about 30 degrees
	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	if (intersection::rayBoxIntersection(p,
		{ (v.x * c - v.y * s) * 0.5f,
						   (v.x * s + v.y * c) * 0.5f,0 },
		center, { m_tileWidth, m_tileHeight,0 },
						   n)) {
		force += n;
	}

	// rotate feeler about -30 degrees
	s = sinf(3.14159f*-0.15f);
	c = cosf(3.14159f*-0.15f);
	if (intersection::rayBoxIntersection(p,
		{ (v.x * c - v.y * s) * 0.5f,
						   (v.x * s + v.y * c) * 0.5f,0 },
		center, { m_tileWidth, m_tileHeight,0 },
						   n)) {
		force += n;
	}
}

glm::vec3 SeekClosestForce::getForce(ai::Entity* entity) const {

	glm::vec3 diff(0);

	ai::Entity* target = nullptr;
	if (entity->getBlackboard().get("target", &target) &&
		target != nullptr) {
		
		// get a vector to the target from "us"
		diff = target->getPosition() - entity->getPosition();

		// if not at the target then move towards them
		if (glm::dot(diff, diff) > 0) 
			diff = glm::normalize(diff);
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return diff * maxForce;
}