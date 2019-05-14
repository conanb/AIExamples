#pragma once

#include "Agent.h"

namespace ai {

enum eBehaviourResult {
	SUCCESS,
	FAILURE,
	RUNNING
};

class Behaviour {
public:

	// empty constructors and destructors for base class
	Behaviour() {}
	virtual ~Behaviour() {}

	// pure virtual function for executing the behaviour
	virtual eBehaviourResult execute(Agent* entity) = 0;

};

} // namespace ai