#include "AIShowcaseApp.h"
#include "Font.h"
#include "Input.h"

#include "aiUtilities.h"

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
	
	m_2dRenderer = new Renderer2D();

	m_font = new Font("./font/consolas.ttf", 32);

	m_map = new Texture("./map/map.bmp");
	m_background = new Texture("./textures/background.png");
	m_spriteSheet = new Texture("./textures/roguelikeSheet_transparent.png");
	m_characterSprites = new Texture("./textures/roguelikeChar_transparent.png");

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

			float x = b->x - a->x;
			float y = b->y - a->y;
			float sqrDist = x * x + y * y;

			if (sqrDist <= (30 * 30)) {
				Pathfinding::Edge* edge = new Pathfinding::Edge();
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

		knight.setPosition(node->x, node->y);

		knight.getBlackboard().set("path", new std::list<Pathfinding::Node*>(), true);
		knight.getBlackboard().set("velocity", new Vector2({ 0 }), true);
		knight.getBlackboard().set("wanderData", new WanderData({ 100,75,25,0,0 }), true);
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
		auto myFunc = new std::function<void(Entity*)>();

		*myFunc = [this](Entity* entity) {
			entity->getBlackboard().set("health", 100.f);
			auto n = m_pathNodes[rand() % m_pathNodes.size()];
			entity->setPosition(n->x, n->y);

			std::list<Pathfinding::Node*>* path = nullptr;
			if (entity->getBlackboard().get("path", &path))
				path->clear();
			Vector2* velocity = nullptr;
			if (entity->getBlackboard().get("velocity", &velocity)) {
				velocity->x = velocity->y = 0;
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
			caveman.setPosition(8 + 16.0f * (index % m_map->getWidth()), 8 + 16.0f * (index / m_map->getWidth()));
		} while (m_tiles[index] != -1);

		caveman.getBlackboard().set("velocity", new Vector2({ 0 }), true);
		caveman.getBlackboard().set("wanderData", new WanderData({100,75,25,0,0}), true);
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

		caveman.getBlackboard().set("respawnFunction", new std::function<void(Entity*)>(
			[this](Entity* entity) {
			entity->getBlackboard().set("health", 100.f);
			
			int i;
			do {
				i = rand() % (m_map->getWidth() * m_map->getHeight());
				entity->setPosition(8 + 16.0f * (i % m_map->getWidth()), 8 + 16.0f * (i / m_map->getWidth()));
			} while (m_tiles[i] != -1);
			Vector2* velocity = nullptr;
			if (entity->getBlackboard().get("velocity", &velocity)) {
				velocity->x = velocity->y = 0;
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

void AIShowcaseApp::update(float deltaTime) {

	for (auto& knight : m_knights)
		knight.executeBehaviours(deltaTime);
	for (auto& caveman : m_cavemen)
		caveman.executeBehaviours(deltaTime);
		
	// input example
	Input* input = Input::getInstance();

	// exit the application
	if (input->isKeyDown(INPUT_KEY_ESCAPE))
		quit();

	// toggle debug rendering
	if (input->wasKeyPressed(INPUT_KEY_D))
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
	float x, y, tx, ty;

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
			m_2dRenderer->drawBox(node->x, node->y, 4, 4);

			for (auto edge : node->edges) {
				MyNode* target = (MyNode*)edge->target;

				m_2dRenderer->drawLine(node->x, node->y, target->x, target->y);
			}
		}
	}

	// draw knights and cavemen
	pw = 1.0f / m_characterSprites->getWidth();
	ph = 1.0f / m_characterSprites->getHeight();
	w = pw * 16.0f;
	h = ph * 16.0f;

	float health, maxHealth;

	Entity* target = nullptr;

	m_2dRenderer->setUVRect(0, 1 - h, w, h);
	for (auto& knight : m_knights) {
		knight.getPosition(&x, &y);
		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawSprite(m_characterSprites, x, y, 16, 16);

		// draw health
		knight.getBlackboard().get("health", health);
		knight.getBlackboard().get("maxHealth", maxHealth);

		m_2dRenderer->setRenderColour(0, 1, 0);
		m_2dRenderer->drawBox(x, y + 12, 16 * (health / maxHealth), 2);
	}

	Vector2* v = nullptr;
	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	float s2 = sinf(3.14159f*-0.15f);
	float c2 = cosf(3.14159f*-0.15f);

	m_2dRenderer->setUVRect(0, h * 6 + ph * 6, w, h);
	for (auto& caveman : m_cavemen) {
		caveman.getPosition(&x, &y);
		m_2dRenderer->setRenderColour(1, 1, 1);
		m_2dRenderer->drawSprite(m_characterSprites, x, y, 16, 16);

		// draw health
		caveman.getBlackboard().get("health", health);
		caveman.getBlackboard().get("maxHealth", maxHealth);

		m_2dRenderer->setRenderColour(0, 1, 0);
		m_2dRenderer->drawBox(x, y + 12, 16 * (health / maxHealth), 2);

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

					m_2dRenderer->drawLine(x, y, x + vx * 40, y + vy * 40);

					m_2dRenderer->drawLine(x, y, x + (vx * c - vy * s) * 40 * 0.5f, y + (vx * s + vy * c) * 40 * 0.5f);
					m_2dRenderer->drawLine(x, y, x + (vx * c2 - vy * s2) * 40 * 0.5f, y + (vx * s2 + vy * c2) * 40 * 0.5f);
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

eBehaviourResult FollowPathBehaviour::execute(Entity* entity, float deltaTime) {

	// access data from the game object
	std::list<Pathfinding::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false ||
		path->empty())
		return eBehaviourResult::FAILURE;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	float x = 0, y = 0;
	entity->getPosition(&x, &y);

	// access first node we're heading towards
	MyNode* first = (MyNode*)path->front();

	// distance to first
	float xDiff = first->x - x;
	float yDiff = first->y - y;

	float distance = xDiff * xDiff + yDiff * yDiff;

	// if not at the target then move towards it
	if (distance > 25) {

		distance = sqrt(distance);
		xDiff /= distance;
		yDiff /= distance;

		// move to target (can overshoot!)
		entity->translate(xDiff * speed * deltaTime, yDiff * speed * deltaTime);
	}
	else {
		// at the node, remove it and move to the next
		path->pop_front();
	}
	return eBehaviourResult::SUCCESS;
}

MyNode* NewPathBehaviour::findClosest(float x, float y) {

	MyNode* closest = nullptr;
	float closestDist = 2000 * 2000;

	for (auto node : m_nodes) {

		float dist = (node->x - x) * (node->x - x) + (node->y - y) * (node->y - y);

		if (dist < closestDist) {
			closest = node;
			closestDist = dist;
		}
	}

	return closest;
}

eBehaviourResult NewPathBehaviour::execute(Entity* entity, float deltaTime) {

	// access data from the game object
	std::list<Pathfinding::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false)
		return eBehaviourResult::FAILURE;

	float x, y;
	entity->getPosition(&x, &y);

	// random end node
	bool found = false;
	do {

		auto start = findClosest(x, y);
		auto end = m_nodes[rand() % m_nodes.size()];

		found = Pathfinding::Search::aStar(start, end, *path, MyNode::heuristic);

	} while (found == false);

	return eBehaviourResult::SUCCESS;
}

Force WaterAvoidanceForce::getForce(Entity* entity) const {

	Force force = { 0, 0 };

	float x, y;
	entity->getPosition(&x, &y);

	Vector2* velocity = nullptr;
	if (entity->getBlackboard().get("velocity", &velocity) == false)
		return force;
	
	// are we moving?
	float mag = velocity->x * velocity->x + velocity->y * velocity->y;
	if (mag > 0) {

		mag = sqrt(mag);

		float velX = velocity->x / mag * m_feelerLength;
		float velY = velocity->y / mag * m_feelerLength;

		// find which cell we're in
		unsigned int cellX = unsigned int(x / m_tileWidth);
		unsigned int cellY = unsigned int(y / m_tileHeight);

		const unsigned char* pixels = m_map->getPixels();

		// test the 8 boxes
		if (cellX > 0 &&
			getPixelColour3(3 * (cellY * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX - 1) * m_tileWidth,
						   cellY * m_tileHeight,
						   force);
		}
		if (cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   cellX * m_tileWidth,
						   (cellY - 1) * m_tileHeight,
						   force);
		}
		if (cellX > 0 &&
			cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX - 1) * m_tileWidth,
						   (cellY - 1) * m_tileHeight,
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			getPixelColour3(3 * (cellY * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX + 1) * m_tileWidth,
						   cellY * m_tileHeight,
						   force);
		}
		if (cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   cellX * m_tileWidth,
						   (cellY + 1) * m_tileHeight,
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX - 1) * m_tileWidth,
						   (cellY + 1) * m_tileHeight,
						   force);
		}
		if (cellX > 0 &&
			cellY < (m_map->getHeight() - 1) &&
			getPixelColour3(3 * ((cellY + 1) * m_map->getWidth() + cellX - 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX - 1) * m_tileWidth,
						   (cellY + 1) * m_tileHeight,
						   force);
		}
		if (cellX < (m_map->getWidth() - 1) &&
			cellY > 0 &&
			getPixelColour3(3 * ((cellY - 1) * m_map->getWidth() + cellX + 1), pixels) == 0x000000ff) {
			collideWithBox(x, y, velX, velY,
						   (cellX + 1) * m_tileWidth,
						   (cellY - 1) * m_tileHeight,
						   force);
		}
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return { force.x * maxForce, force.y * maxForce };
}

void WaterAvoidanceForce::collideWithBox(float x, float y, float vx, float vy, 
										 float boxX, float boxY,
										 Force& force) const {

	float nx = 0, ny = 0;
	if (rayBoxIntersection(x, y,
						   vx, vy,
						   boxX, boxY, m_tileWidth, m_tileHeight,
						   nx, ny)) {
		force.x += nx;
		force.y += ny;
	}

	// rotate feeler about 30 degrees
	float s = sinf(3.14159f*0.15f);
	float c = cosf(3.14159f*0.15f);
	if (rayBoxIntersection(x, y,
						   (vx * c - vy * s) * 0.5f,
						   (vx * s + vy * c) * 0.5f,
						   boxX, boxY, m_tileWidth, m_tileHeight,
						   nx, ny)) {
		force.x += nx;
		force.y += ny;
	}

	// rotate feeler about -30 degrees
	s = sinf(3.14159f*-0.15f);
	c = cosf(3.14159f*-0.15f);
	if (rayBoxIntersection(x, y,
						   (vx * c - vy * s) * 0.5f,
						   (vx * s + vy * c) * 0.5f,
						   boxX, boxY, m_tileWidth, m_tileHeight,
						   nx, ny)) {
		force.x += nx;
		force.y += ny;
	}
}

Force SeekClosestForce::getForce(Entity* entity) const {

	float xDiff = 0;
	float yDiff = 0;

	Entity* target = nullptr;
	if (entity->getBlackboard().get("target", &target) &&
		target != nullptr) {

		// get target position
		float targetX = 0, targetY = 0;
		target->getPosition(&targetX, &targetY);

		// get my position
		float x = 0, y = 0;
		entity->getPosition(&x, &y);

		// get a vector to the target from "us"
		xDiff = targetX - x;
		yDiff = targetY - y;
		float distance = (xDiff * xDiff + yDiff * yDiff);

		// if not at the target then move towards them
		if (distance > 0) {

			distance = sqrt(distance);

			// need to make the difference the length of 1
			// this is so movement can be "pixels per second"
			xDiff /= distance;
			yDiff /= distance;
		}
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}