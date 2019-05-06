#pragma once

#include "Entity.h"

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
	virtual eBehaviourResult execute(Entity* entity, float deltaTime) = 0;

};

} // namespace ai