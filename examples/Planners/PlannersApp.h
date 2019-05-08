#pragma once

#include "Application.h"
#include "Renderer2D.h"
#include "Planner.h"

#include <deque>
#include <vector>
#include <map>

enum eContainer {
	RED,
	GREEN,
	BLUE
};

class DWRState : public Planner::State {
public:

	DWRState() {}
	DWRState(const DWRState& state) {
		stack[0] = state.stack[0];
		stack[1] = state.stack[1];
		stack[2] = state.stack[2];
		id = state.id;
	}
	virtual ~DWRState() {}

	std::deque<eContainer>	stack[3];

	// converts the state of the containers to a single
	// unsigned int, stores it in 'id' and returns it
	unsigned int hash();
};

class DWRAction : public Planner::Action {
public:

	DWRAction(eContainer c, int s, int e) : colour(c), start(s), end(e) {}
	virtual ~DWRAction() {}

	virtual bool conditionMet(Planner::State* state) const {
		return ((DWRState*)state)->stack[start].empty() == false &&
			((DWRState*)state)->stack[start].back() == colour;
	}

	virtual Planner::State* execute(Planner::State* state) const {
		if (!conditionMet(state))
			return nullptr;

		DWRState* newState = new DWRState(*(DWRState*)state);
		newState->stack[start].pop_back();
		newState->stack[end].push_back(colour);
		newState->hash();
		return newState;
	}

	eContainer colour;
	int start, end;
};

class PlannersApp : public app::Application {
public:

	PlannersApp();
	virtual ~PlannersApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update();
	virtual void draw();

protected:

	app::Renderer2D*	m_2dRenderer;
	app::Font*		m_font;

	unsigned int m_currentStateID;

	// all states and actions
	std::map<unsigned int, DWRState*> m_states;
	std::vector<DWRAction> m_actions;

	// stored as a member variable within the application class
	std::list<DWRAction*> m_pathActions;
};