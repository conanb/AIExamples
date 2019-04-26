#pragma once

#include <vector>
#include <list>
#include <functional>

namespace Pathfinding {

class Node;

// a one-way link to a node with a cost
class Edge {
public:

	// example of potentially adding flags to edges
	enum eFlags {
		CLOSED = (1 << 0),
		RIVER = (1 << 1),
	};

	Edge() : flags(0) {}
	Edge(Node* t, float c) : target(t), cost(c), flags(0) {}
	virtual ~Edge() {}

	Node* target;
	float cost;

	unsigned int flags;
};

// a node for any type of graph search
// specific implementation details (such as position etc)
// is implemented in a derived class
class Node {
public:

	// potentially tag nodes with flags to search for
	enum eFlags {
		MEDKIT = (1 << 0),
		AMMO = (1 << 1),
	};

	Node() : flags(0) {}
	virtual ~Node() { for (auto e : edges) delete e; }

	std::vector<Edge*> edges;

	unsigned int flags;

	// search data
	float hScore;
	float fScore;
	float gScore;
	Node* previous;
	
	static bool compareGScore(Node* a, Node* b) {
		return a->gScore < b->gScore;
	}

	static bool compareFScore(Node* a, Node* b) {
		return a->fScore < b->fScore;
	}
};

// the idea is that the search data stored in Node could potentially move
// into this struct so that multiple searches can be run on the same graph
// simultaneously using threading, but for now this is just a container
// for static methods
class Search {
public:

	// Dijkstra's Shortest Path methods
	static bool dijkstra(Node* start, Node* end, std::list<Node*>& path);

	// will search for the closest node that has all of the requested flags
	static bool dijkstraFindFlags(Node* start, unsigned int flags, std::list<Node*>& path);

	// A* methods
	typedef std::function<float(Node* a, Node* b)> HeuristicCheck;

	static bool aStar(Node* start, Node* end, std::list<Node*>& path, HeuristicCheck heuristic);

private:

	Search() {}
};

}