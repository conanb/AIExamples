#include "NavMesh.h"
#include "Entity.h"

#include "poly2tri/poly2tri.h"

NavMesh::NavMesh(float width, float height) {

	m_polygons.push_back({});
	m_polygons[0].push_back(new p2t::Point(0, 0));
	m_polygons[0].push_back(new p2t::Point(0, height));
	m_polygons[0].push_back(new p2t::Point(width, height));
	m_polygons[0].push_back(new p2t::Point(width, 0));

	m_cdt = new p2t::CDT(m_polygons[0]);
}

NavMesh::~NavMesh() {
	for (auto node : m_nodes)
		delete node;
}

NavMesh::Node* NavMesh::getRandomNode() const {
	if (m_nodes.empty())
		return nullptr;

	return m_nodes[rand() % m_nodes.size()];
}

NavMesh::Node* NavMesh::findClosest(float x, float y) const {

	NavMesh::Node* closest = nullptr;
	float closestDist = 2000 * 2000;

	for (auto node : m_nodes) {

		float dist = (node->position.x - x) * (node->position.x - x) + (node->position.y - y) * (node->position.y - y);

		if (dist < closestDist) {
			closest = node;
			closestDist = dist;
		}
	}

	return closest;
}

bool NavMesh::addObstacle(float x, float y, float width, float height, float padding) {

	for (auto& ob : m_obstacles) {
		if (((ob.x + ob.w + ob.padding) < x - padding ||
			 (ob.y + ob.h + ob.padding) < y - padding ||
			 (x + width + padding) < ob.x - ob.padding ||
			 (y + height + padding) < ob.y - ob.padding) == false)
			return false;
	}

	Obstacle o = { x, y, width, height, padding };
	m_obstacles.push_back(o);

	m_polygons.push_back({});
	m_polygons.back().push_back(new p2t::Point(o.x - padding, o.y - padding));
	m_polygons.back().push_back(new p2t::Point(o.x - padding, o.y + o.h + padding));
	m_polygons.back().push_back(new p2t::Point(o.x + o.w + padding, o.y + o.h + padding));
	m_polygons.back().push_back(new p2t::Point(o.x + o.w + padding, o.y - padding));
	m_cdt->AddHole(m_polygons.back());

	return true;
}

void NavMesh::build() {

	m_cdt->Triangulate();

	// first convert triangles to NavMesh::Node's
	std::vector<p2t::Triangle*> triangles = m_cdt->GetTriangles();
	for (auto tri : triangles) {

		auto node = new NavMesh::Node();

		node->vertices.push_back({ (float)tri->GetPoint(0)->x, (float)tri->GetPoint(0)->y });
		node->vertices.push_back({ (float)tri->GetPoint(1)->x, (float)tri->GetPoint(1)->y });
		node->vertices.push_back({ (float)tri->GetPoint(2)->x, (float)tri->GetPoint(2)->y });

		node->position.x = (node->vertices[0].x + node->vertices[1].x + node->vertices[2].x) / 3;
		node->position.y = (node->vertices[0].y + node->vertices[1].y + node->vertices[2].y) / 3;

		m_nodes.push_back(node);
	}

	// then link nodes that share triangle edges
	for (auto n : m_nodes) {
		for (auto n2 : m_nodes) {

			if (n == n2) continue;

			Vector2 v[2];
			if (n->getAdjacentVertices(n2, v) == 2) {

				float mag = (n->position.x - n2->position.x) *
					(n->position.x - n2->position.x) +
					(n->position.y - n2->position.y) *
					(n->position.y - n2->position.y);

				n->edges.push_back(new Pathfinding::Edge(n2, 1));
				n2->edges.push_back(new Pathfinding::Edge(n, 1));
			}
		}
	}

	// cleanup polygons
	for (auto& p : m_polygons)
		for (auto ptr : p)
			delete ptr;
	m_polygons.clear();

	// close up Poly2Tri
	delete m_cdt;
	m_cdt = nullptr;
}

eBehaviourResult NavMesh::FollowPathBehaviour::execute(Entity* entity, float deltaTime) {

	// access data from the game object
	std::list<Vector2>* smoothPath = nullptr;
	if (entity->getBlackboard().get("smoothpath", &smoothPath) == false ||
		smoothPath->empty())
		return eBehaviourResult::FAILURE;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	float x = 0, y = 0;
	entity->getPosition(&x, &y);

	// access first node we're heading towards
	Vector2 first = smoothPath->front();

	// distance to first
	float xDiff = first.x - x;
	float yDiff = first.y - y;

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
		smoothPath->pop_front();
	}
	return eBehaviourResult::SUCCESS;
}

eBehaviourResult NavMesh::NewPathBehaviour::execute(Entity* entity, float deltaTime) {

	// access data from the game object
	std::list<Pathfinding::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false)
		return eBehaviourResult::FAILURE;

	std::list<Vector2>* smoothPath = nullptr;
	if (entity->getBlackboard().get("smoothpath", &smoothPath) == false)
		return eBehaviourResult::FAILURE;

	float x, y;
	entity->getPosition(&x, &y);

	// random end node
	bool found = false;
	do {

		auto first = m_navMesh->findClosest(x, y);
		auto end = first;
		while (end == first)
			end = m_navMesh->getRandomNode();

		found = Pathfinding::Search::aStar(first, end, *path, NavMesh::Node::heuristic);

	} while (found == false);

	NavMesh::smoothPath(*path, *smoothPath);

	return eBehaviourResult::SUCCESS;
}

int NavMesh::stringPull(const Vector2* portals, int portalCount,
						Vector2* points, const int maxPoints) {

	// Find straight path
	int npts = 0;

	// Init scan state
	Vector2 portalApex, portalLeft, portalRight;
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	portalApex = portals[0];
	portalLeft = portals[0];
	portalRight = portals[1];

	// Add start point
	points[npts] = portalApex;
	npts++;

	for (int i = 1; i < portalCount && npts < maxPoints; ++i) {
		Vector2 left = portals[i * 2 + 0];
		Vector2 right = portals[i * 2 + 1];

		// Update right vertex
		if (triarea2(portalApex, portalRight, right) <= 0.0f) {
			if (vequal(portalApex, portalRight) || triarea2(portalApex, portalLeft, right) > 0.0f) {

				// Tighten the funnel
				portalRight = right;
				rightIndex = i;
			}
			else {
				// Right over left, insert left to path and restart scan from portal left point
				points[npts] = portalLeft;
				npts++;

				// Make current left the new apex
				portalApex = portalLeft;
				apexIndex = leftIndex;

				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;

				// Restart scan
				i = apexIndex;
				continue;
			}
		}

		// Update left vertex
		if (triarea2(portalApex, portalLeft, left) >= 0.0f) {
			if (vequal(portalApex, portalLeft) || triarea2(portalApex, portalRight, left) < 0.0f) {

				// Tighten the funnel
				portalLeft = left;
				leftIndex = i;
			}
			else {
				// Left over right, insert right to path and restart scan from portal right point
				points[npts] = portalRight;
				npts++;

				// Make current right the new apex
				portalApex = portalRight;
				apexIndex = rightIndex;

				// Reset portal
				portalLeft = portalApex;
				portalRight = portalApex;
				leftIndex = apexIndex;
				rightIndex = apexIndex;

				// Restart scan
				i = apexIndex;
				continue;
			}
		}
	}

	// Append last point to path
	if (npts < maxPoints) {
		points[npts] = portals[(portalCount - 1) * 2 + 0];
		npts++;
	}

	return npts;
}

int NavMesh::smoothPath(const std::list<Pathfinding::Node*>& path,
						std::list<Vector2>& smoothPath) {

	if (path.size() == 0)
		return 0;

	smoothPath.clear();

	Vector2* portals = new Vector2[(path.size() + 1) * 2];

	int index = 0;

	// add start to portals
	portals[index++] = ((NavMesh::Node*)path.front())->position;
	portals[index++] = ((NavMesh::Node*)path.front())->position;

	NavMesh::Node* prev = nullptr;
	for (auto n : path) {

		NavMesh::Node* node = (NavMesh::Node*)n;

		if (prev != nullptr) {

			// 
			Vector2 adj[2];
			prev->getAdjacentVertices(node, adj);

			Vector2 fromPrev = { node->position.x - prev->position.x,
								 node->position.y - prev->position.y };

			Vector2 toAdj0 = { adj[0].x - prev->position.x,
				adj[0].y - prev->position.y };

			if ((fromPrev.x * toAdj0.y - toAdj0.x * fromPrev.y) > 0) {
				portals[index++] = adj[0];
				portals[index++] = adj[1];
			}
			else {
				portals[index++] = adj[1];
				portals[index++] = adj[0];
			}
		}

		prev = node;
	}

	// add end to portals
	portals[index++] = ((NavMesh::Node*)path.back())->position;
	portals[index++] = ((NavMesh::Node*)path.back())->position;

	// shorten path through portals
	Vector2 out[100];
	int count = stringPull(portals, index / 2, out, 100);

	for (int i = 0; i < count; ++i)
		smoothPath.push_back(out[i]);

	delete[] portals;
	
	return smoothPath.size();
}