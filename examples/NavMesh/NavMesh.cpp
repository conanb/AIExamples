#include "NavMesh.h"
#include "Entity.h"
#include "Timing.h"
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

NavMesh::Node* NavMesh::findClosest(const glm::vec3& p) const {

	NavMesh::Node* closest = nullptr;
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

		node->vertices.push_back({ (float)tri->GetPoint(0)->x, (float)tri->GetPoint(0)->y, 0 });
		node->vertices.push_back({ (float)tri->GetPoint(1)->x, (float)tri->GetPoint(1)->y, 0 });
		node->vertices.push_back({ (float)tri->GetPoint(2)->x, (float)tri->GetPoint(2)->y, 0 });

		node->position.x = (node->vertices[0].x + node->vertices[1].x + node->vertices[2].x) / 3;
		node->position.y = (node->vertices[0].y + node->vertices[1].y + node->vertices[2].y) / 3;

		m_nodes.push_back(node);
	}

	// then link nodes that share triangle edges
	for (auto n : m_nodes) {
		for (auto n2 : m_nodes) {

			if (n == n2) continue;

			glm::vec3 v[2];
			if (n->getAdjacentVertices(n2, v) == 2) {

				float mag = (n->position.x - n2->position.x) *
					(n->position.x - n2->position.x) +
					(n->position.y - n2->position.y) *
					(n->position.y - n2->position.y);

				n->edges.push_back(new graph::Edge(n2, 1));
				n2->edges.push_back(new graph::Edge(n, 1));
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

ai::eBehaviourResult NavMesh::FollowPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<glm::vec3>* smoothPath = nullptr;
	if (entity->getBlackboard().get("smoothpath", &smoothPath) == false ||
		smoothPath->empty())
		return ai::eBehaviourResult::FAILURE;

	float speed = 0;
	entity->getBlackboard().get("speed", speed);

	auto position = entity->getPosition();

	// access first node we're heading towards
	glm::vec3 first = smoothPath->front();

	// distance to first
	auto diff = first - position;
	
	// if not at the target then move towards it
	if (glm::dot(diff, diff) > 25) {
		
		// move to target (can overshoot!)
		entity->translate(glm::normalize(diff) * speed * app::Time::deltaTime());
	}
	else {
		// at the node, remove it and move to the next
		smoothPath->pop_front();
	}
	return ai::eBehaviourResult::SUCCESS;
}

ai::eBehaviourResult NavMesh::NewPathBehaviour::execute(ai::Entity* entity) {

	// access data from the game object
	std::list<graph::Node*>* path = nullptr;
	if (entity->getBlackboard().get("path", &path) == false)
		return ai::eBehaviourResult::FAILURE;

	std::list<glm::vec3>* smoothPath = nullptr;
	if (entity->getBlackboard().get("smoothpath", &smoothPath) == false)
		return ai::eBehaviourResult::FAILURE;

	auto position = entity->getPosition();

	// random end node
	bool found = false;
	do {

		auto first = m_navMesh->findClosest(position);
		auto end = first;
		while (end == first)
			end = m_navMesh->getRandomNode();

		found = graph::Search::aStar(first, end, *path, NavMesh::Node::heuristic);

	} while (found == false);

	NavMesh::smoothPath(*path, *smoothPath);

	return ai::eBehaviourResult::SUCCESS;
}

int NavMesh::stringPull(const glm::vec3* portals, int portalCount,
	glm::vec3* points, const int maxPoints) {

	// Find straight path
	int npts = 0;

	// Init scan state
	glm::vec3 portalApex, portalLeft, portalRight;
	int apexIndex = 0, leftIndex = 0, rightIndex = 0;
	portalApex = portals[0];
	portalLeft = portals[0];
	portalRight = portals[1];

	// Add start point
	points[npts] = portalApex;
	npts++;

	for (int i = 1; i < portalCount && npts < maxPoints; ++i) {
		glm::vec3 left = portals[i * 2 + 0];
		glm::vec3 right = portals[i * 2 + 1];

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

int NavMesh::smoothPath(const std::list<graph::Node*>& path,
						std::list<glm::vec3>& smoothPath) {

	if (path.size() == 0)
		return 0;

	smoothPath.clear();

	glm::vec3* portals = new glm::vec3[(path.size() + 1) * 2];

	int index = 0;

	// add start to portals
	portals[index++] = ((NavMesh::Node*)path.front())->position;
	portals[index++] = ((NavMesh::Node*)path.front())->position;

	NavMesh::Node* prev = nullptr;
	for (auto n : path) {

		NavMesh::Node* node = (NavMesh::Node*)n;

		if (prev != nullptr) {

			// 
			glm::vec3 adj[2];
			prev->getAdjacentVertices(node, adj);

			glm::vec3 fromPrev = node->position - prev->position;

			glm::vec3 toAdj0 = adj[0] - prev->position;

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
	glm::vec3 out[100];
	int count = stringPull(portals, index / 2, out, 100);

	for (int i = 0; i < count; ++i)
		smoothPath.push_back(out[i]);

	delete[] portals;
	
	return smoothPath.size();
}