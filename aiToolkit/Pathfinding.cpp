#include "Pathfinding.h"

#include <set>

namespace graph {

bool Search::dijkstra(Node* start, Node* end, std::list<Node*>& path) {

	std::list<Node*> openList;
	std::set<Node*> closedList;

	path.clear();

	start->previous = nullptr;
	start->gScore = 0;

	end->previous = nullptr;

	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		openList.sort(Node::compareGScore);

		Node* current = openList.front();

		if (current == end)
			break;

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList
		for (auto edge : current->edges) {

			Node* target = edge->target;
			float gScore = current->gScore + edge->cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to open list
					target->previous = current;
					target->gScore = gScore;
					openList.push_back(target);
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;
					target->previous = current;
				}
			}
		}
	}

	// did we find a path?
	if (end->previous != nullptr) {
		// path found!
		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}
	
	return false;
}

bool Search::dijkstraFindFlags(Node* start, unsigned int flags, std::list<Node*>& path) {

	std::list<Node*> openList;
	std::set<Node*> closedList;

	path.clear();

	start->previous = nullptr;
	start->gScore = 0;

	Node* end = nullptr;
	
	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		openList.sort(Node::compareGScore);

		Node* current = openList.front();

		// must contain all of the requested flags
		if ((current->flags & flags) == flags) {
			end = current;
			break;
		}

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList
		for (auto edge : current->edges) {

			Node* target = edge->target;
			float gScore = current->gScore + edge->cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to open list
					target->previous = current;
					target->gScore = gScore;
					openList.push_back(target);
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;
					target->previous = current;
				}
			}
		}
	}

	// did we find a path?
	if (end->previous != nullptr) {
		// path found!
		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}

	return false;
}

bool Search::aStar(Node* start, Node* end, std::list<Node*>& path, HeuristicCheck heuristic) {

	if (start == nullptr ||
		end == nullptr)
		return false;

	std::list<Node*> openList;
	std::set<Node*> closedList;

	path.clear();

	start->previous = nullptr;
	start->gScore = 0;
	start->hScore = heuristic(start, end);
	start->fScore = start->gScore + start->fScore;

	end->previous = nullptr;

	openList.push_front(start);

	// do search
	while (openList.empty() == false) {

		// compare with F rather than G
		openList.sort(Node::compareFScore);

		Node* current = openList.front();

		if (current == end)
			break;

		openList.pop_front();
		closedList.insert(current);

		// add all connections to openList
		for (auto edge : current->edges) {

			Node* target = edge->target;
			float gScore = current->gScore + edge->cost;

			// is it already closed?
			if (closedList.find(target) == closedList.end()) {

				auto iter = std::find(openList.begin(), openList.end(), target);
				if (iter == openList.end()) {
					// add to open list
					target->previous = current;

					target->gScore = gScore;

					// include heuristic and final cost
					target->hScore = heuristic(target, end);
					target->fScore = target->gScore + target->hScore;

					openList.push_back(target);
				}
				else if (gScore < target->gScore) {
					target->gScore = gScore;

					// update final cost
					target->fScore = target->gScore + target->hScore;

					target->previous = current;
				}
			}
		}
	}

	// did we find a path?
	if (end->previous != nullptr) {
		// path found!
		while (end != nullptr) {
			path.push_front(end);
			end = end->previous;
		}

		return true;
	}

	return false;
}

} // namespace graph