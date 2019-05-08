#include "PlannersApp.h"
#include "Font.h"
#include "Input.h"
#include "Timing.h"
#include <bitset>

PlannersApp::PlannersApp() {

}

PlannersApp::~PlannersApp() {

}

bool PlannersApp::startup() {
	
	m_2dRenderer = new app::Renderer2D();

	m_font = new app::Font("./font/consolas.ttf", 32);

	// starting state
	auto startState = new DWRState();
	startState->stack[0].push_back(eContainer::BLUE);
	startState->stack[0].push_back(eContainer::GREEN);
	startState->stack[0].push_back(eContainer::RED);
	startState->hash();

	// add to state list
	m_states[startState->id] = startState;

	// current state
	m_currentStateID = startState->id;
	
	// create all potential actions
	m_actions.push_back(DWRAction(eContainer::RED, 0, 1));
	m_actions.push_back(DWRAction(eContainer::RED, 0, 2));
	m_actions.push_back(DWRAction(eContainer::RED, 1, 0));
	m_actions.push_back(DWRAction(eContainer::RED, 1, 2));
	m_actions.push_back(DWRAction(eContainer::RED, 2, 0));
	m_actions.push_back(DWRAction(eContainer::RED, 2, 1));
	m_actions.push_back(DWRAction(eContainer::GREEN, 0, 1));
	m_actions.push_back(DWRAction(eContainer::GREEN, 0, 2));
	m_actions.push_back(DWRAction(eContainer::GREEN, 1, 0));
	m_actions.push_back(DWRAction(eContainer::GREEN, 1, 2));
	m_actions.push_back(DWRAction(eContainer::GREEN, 2, 0));
	m_actions.push_back(DWRAction(eContainer::GREEN, 2, 1));
	m_actions.push_back(DWRAction(eContainer::BLUE, 0, 1));
	m_actions.push_back(DWRAction(eContainer::BLUE, 0, 2));
	m_actions.push_back(DWRAction(eContainer::BLUE, 1, 0));
	m_actions.push_back(DWRAction(eContainer::BLUE, 1, 2));
	m_actions.push_back(DWRAction(eContainer::BLUE, 2, 0));
	m_actions.push_back(DWRAction(eContainer::BLUE, 2, 1));

	// create domain graph
	std::list<Planner::State*> toProcess;
	toProcess.push_back(m_states[m_currentStateID]);

	while (toProcess.empty() == false) {

		// remove a state
		auto state = toProcess.back();
		toProcess.pop_back();

		// get potential actions for the state
		for (auto& action : m_actions) {
			if (action.conditionMet(state)) {

				// create new state
				auto newState = action.execute(state);

				// check if new state already exists
				auto iter = m_states.find(newState->id);
				if (iter == m_states.end()) {

					// if it doesn't then add it to collection
					m_states[newState->id] = (DWRState*)newState;

					// create a link from current state to new state
					state->edges.push_back(new Planner::ActionLink(newState, &action));

					// add new state to be processed
					toProcess.push_back(newState);
				}
				else {
					// state already exists
					delete newState;

					// create link from current to existing state
					state->edges.push_back(new Planner::ActionLink(iter->second, &action));
				}
			}
		}
	}

	auto goalState = new DWRState();
	goalState->stack[1].push_back(eContainer::BLUE);
	goalState->stack[2].push_back(eContainer::GREEN);
	goalState->stack[2].push_back(eContainer::RED);
	goalState->hash();

	std::list<graph::Node*> path;
	graph::Search::dijkstra(
		m_states[m_currentStateID], 
		m_states[goalState->id],
		path);

	delete goalState;

	// collect actions
	auto state = path.front();
	path.pop_front();

	while (path.empty() == false) {
		for (auto& edge : state->edges) {
			if (edge->target == path.front()) {

				m_pathActions.push_back(
					(DWRAction*)((Planner::ActionLink*)edge)->action
					);
			}
		}

		state = path.front();
		path.pop_front();
	}

	return true;
}

void PlannersApp::shutdown() {

	delete m_font;
	delete m_2dRenderer;
}

void PlannersApp::update() {

	// input example
	app::Input* input = app::Input::getInstance();

	// exit the application
	if (input->isKeyDown(app::INPUT_KEY_ESCAPE))
		quit();

	// every second it performs an action
	static float timer = 0;
	timer += app::Time::deltaTime();
	if (timer >= 1) {
		timer -= 1;

		if (m_pathActions.empty() == false) {

			auto action = m_pathActions.front();
			m_pathActions.pop_front();

			auto state = action->execute(m_states[m_currentStateID]);

			m_currentStateID = state->id;
			
			delete state;
		}

		/*// find all actions that we can perform
		std::vector<DWRAction> potentialActions;
		for (auto& action : m_actions) {
			if (action.conditionMet(m_states[m_currentStateID]))
				potentialActions.push_back(action);
		}

		// pick a random action and execute it
		if (potentialActions.empty() == false) {

			auto& action = potentialActions[rand() % potentialActions.size()];
			auto state = action.execute(m_states[m_currentStateID]);

			m_currentStateID = state->id;

			// add new state to list if it doesn't exist, or delete it
			if (m_states.find(state->id) == m_states.end())
				m_states[state->id] = (DWRState*)state;
			else
				delete state;
		}*/
	}
}

void PlannersApp::draw() {

	// wipe the screen to the background colour
	clearScreen();

	// begin drawing sprites
	m_2dRenderer->begin();

	// draws the containers of the current state
	for (int i = 0, j = 0; i < 3; ++i, j = 0) {
		for (auto& c : m_states[m_currentStateID]->stack[i]) {
			switch (c) {
			case eContainer::RED:	m_2dRenderer->setRenderColour(1, 0, 0);	break;
			case eContainer::GREEN:	m_2dRenderer->setRenderColour(0, 1, 0);	break;
			case eContainer::BLUE:	m_2dRenderer->setRenderColour(0, 0, 1);	break;
			};
			m_2dRenderer->drawBox(getWindowWidth() / 2 - 300 + i * 300.0f, 180 + j * 150.0f, 150, 150);
			++j;
		}
	}

	// draw a line under the containers
	m_2dRenderer->setRenderColour(1, 1, 0);
	m_2dRenderer->drawBox(getWindowWidth() * 0.5f, 100, getWindowWidth() / 7.0f * 5, 10);

	// output some text
	char buf[256];
	sprintf_s(buf, "States found: %i", m_states.size());
	m_2dRenderer->drawText(m_font, buf, 0, 0);

	// done drawing sprites
	m_2dRenderer->end();
}

unsigned int DWRState::hash() {
	// converts a state to an unsigned int
	// each colour is a combination of 2-bits
	// red = 11
	// green = 01
	// blue = 10
	// none = 00
	// each stack is then 6-bits (3 containers)
	// total is 18-bits (whole combination stored in an unsigned int 32-bit)
	std::bitset<32> set(0);
	for (unsigned int i = 0, j = 0; i < 3; ++i, j = 0) {
		for (auto& c : stack[i]) {			
			if (c == eContainer::RED) {
				set[i * 6 + j * 2] = true;
				set[i * 6 + j * 2 + 1] = true;
			}
			else if (c == eContainer::GREEN) {
				set[i * 6 + j * 2] = true;
			}
			else if (c == eContainer::BLUE) {
				set[i * 6 + j * 2 + 1] = true;
			}
			++j;
		}
	}
	id = set.to_ulong();
	return id;
}