#pragma once

#include "Pathfinding.h"

namespace Planner {

	class State : public graph::Node {
	public:
		State() : id(0) {}
		virtual ~State() {}

		// a way of identifying each state
		unsigned int id;
	};

	class Action {
	public:
		Action() {}
		virtual ~Action() {}
		virtual bool conditionMet(State* state) const = 0;
		virtual State* execute(State* state) const = 0;
	};

	class ActionLink : public graph::Edge {
	public:

		ActionLink(State* t, Action* a) : Edge(t, 1), action(a) {}
		virtual ~ActionLink() {}

		Action*	action;
	};
}