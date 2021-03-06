#pragma once

#include "Pathfinding.h"
#include "Behaviour.h"
#include "Condition.h"

// forward declaring some Poly2Tri objects
namespace p2t {
	class CDT;
	struct Point;
}

namespace ai {
	class Agent;
}

class NavMesh {
public:

	// creates the base walkable area
	NavMesh(float width, float height);
	~NavMesh();

	struct Obstacle {
		float x, y, w, h, padding;
	};

	// returns true if the obstacle was added safely (does not overlap)
	//bool addObstacle(const Obstacle& obstacle, float padding);
	bool addObstacle(float x, float y, float width, float height, float padding);

	// triangulates the walkable area and builds the nav mesh nodes
	void build();

	// given a list of pathfinding nodes from a pathfinding search,
	// treat them as NavMesh::Node's and smooth the path using a
	// funneling algorithm from (http://digestingduck.blogspot.com.au/2010/03/simple-stupid-funnel-algorithm.html)
	// we must build a list of portals first
	static size_t smoothPath(const std::list<graph::Node*>& path, std::list<glm::vec3>& smoothPath);

	// access nodes
	class Node;
	size_t getNodeCount() const { return m_nodes.size(); }
	const std::vector<NavMesh::Node*>& getNodes() const { return m_nodes; }

	// access random node or closest node to a coordinate
	NavMesh::Node* getRandomNode() const;
	NavMesh::Node* findClosest(const glm::vec3& p) const;

	// access osbatcles
	size_t getObstacleCount() const { return m_obstacles.size(); }
	const std::vector<Obstacle>& getObstacles() const { return m_obstacles; }

public:

	// the nav mesh node
	class Node : public graph::Node {
	public:

		Node() : position(0,0,0) {}
		virtual ~Node() {}

		glm::vec3 position;

		// edges
		std::vector<glm::vec3> vertices;

		// finds any vertices that are shared with the other node
		int getAdjacentVertices(NavMesh::Node* other, glm::vec3* adjacent) {
			int count = 0;
			for (auto v : vertices) {
				for (auto v2 : other->vertices) {
					if (v.x == v2.x &&
						v.y == v2.y) {
						adjacent[count++] = v;
						break;
					}
				}
			}
			return count;
		}

		// simple distance squared
		static float heuristic(graph::Node* a, graph::Node* b) {
			NavMesh::Node* s = (NavMesh::Node*)a;
			NavMesh::Node* e = (NavMesh::Node*)b;
			float x = e->position.x - s->position.x;
			float y = e->position.y - s->position.y;
			return x * x + y * y;
		}
	};

	// a behaviour that follows a path, then randomly picks
	// a new target node when it reaches the end of the path
	class FollowPathBehaviour : public ai::Behaviour {
	public:

		FollowPathBehaviour() {}
		virtual ~FollowPathBehaviour() {}

		virtual ai::eBehaviourResult execute(ai::Agent* entity);
	};

	// a behaviour that finds a new path and smooths it
	class NewPathBehaviour : public ai::Behaviour {
	public:

		NewPathBehaviour(NavMesh* navMesh) : m_navMesh(navMesh) {}
		virtual ~NewPathBehaviour() {}

		virtual ai::eBehaviourResult execute(ai::Agent* entity);

	protected:

		NavMesh* m_navMesh;
	};

protected:

	// funneling algorithm from (http://digestingduck.blogspot.com.au/2010/03/simple-stupid-funnel-algorithm.html)
	inline static float triarea2(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		const float ax = b.x - a.x;
		const float ay = b.y - a.y;
		const float bx = c.x - a.x;
		const float by = c.y - a.y;
		return bx*ay - ax*by;
	}
	inline static float vdistsqr(const glm::vec3& a, const glm::vec3& b) {
		float dx = a.x - b.x;
		float dy = a.y - b.y;
		return (dx*dx) + (dy*dy);
	}
	inline static bool vequal(const glm::vec3& a, const glm::vec3& b) {
		static const float eq = 0.001f*0.001f;
		return vdistsqr(a, b) < eq;
	}
	static int stringPull(const glm::vec3* portals, int portalCount, glm::vec3* points, const int maxPoints);

	std::vector<Obstacle> m_obstacles;
	std::vector<NavMesh::Node*> m_nodes;

	// this is used for building the mesh
	// uses Poly2Tri (https://github.com/greenm01/poly2tri)
	p2t::CDT* m_cdt;
	std::vector<std::vector<p2t::Point*>> m_polygons;
};