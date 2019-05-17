#pragma once

#include "State.h"
#include <glm/glm.hpp>

namespace ai {

struct WanderData {
	float offset;
	float radius;
	float jitter;
	glm::vec3 target;
	glm::vec3 axisWeights = { 1,1,1 };
};

// abstract class
class SteeringForce {
public:

	SteeringForce() {}
	virtual ~SteeringForce() {}

	// pure virtual function
	virtual glm::vec3 getForce(Agent* entity) const = 0;
};

// weighted steering force
struct WeightedForce {
	SteeringForce* force;
	float weight;
};

// steering behaviour
class SteeringBehaviour : public Behaviour {
public:

	SteeringBehaviour() {}
	virtual ~SteeringBehaviour() {}

	void addForce(SteeringForce* force, float weight = 1.0f) {
		WeightedForce wf = { force, weight };
		m_forces.push_back(wf);
	}

	void setWeightForForce(SteeringForce* force, float weight) {
		for (auto& wf : m_forces) {
			if (wf.force == force)
				wf.weight = weight;
		}
	}

	virtual eBehaviourResult execute(Agent* entity);

protected:

	std::vector<WeightedForce>	m_forces;
};

// steering behaviour state for a FSM
class SteeringState : public State {
public:

	SteeringState() {}
	virtual ~SteeringState() {}

	void addForce(SteeringForce* force, float weight = 1.0f) {
		WeightedForce wf = { force, weight };
		m_forces.push_back(wf);
	}

	void setWeightForForce(SteeringForce* force, float weight) {
		for (auto& wf : m_forces) {
			if (wf.force == force)
				wf.weight = weight;
		}
	}

	virtual void update(Agent* entity);

protected:

	std::vector<WeightedForce>	m_forces;
};

class SeekForce : public SteeringForce {
public:

	SeekForce(Agent* target = nullptr) : m_target(target) {}
	virtual ~SeekForce() {}

	void setTarget(Agent* target) { m_target = target; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	Agent*	m_target;
};

class FleeForce : public SteeringForce {
public:

	FleeForce(Agent* target = nullptr) : m_target(target) {}
	virtual ~FleeForce() {}

	void setTarget(Agent* target) { m_target = target; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	Agent*	m_target;
};

class PursueForce : public SteeringForce {
public:

	PursueForce(Agent* target = nullptr) : m_target(target) {}
	virtual ~PursueForce() {}

	void setTarget(Agent* target) { m_target = target; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	Agent*	m_target;
};

class EvadeForce : public SteeringForce {
public:

	EvadeForce(Agent* target = nullptr) : m_target(target) {}
	virtual ~EvadeForce() {}

	void setTarget(Agent* target) { m_target = target; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	Agent*	m_target;
};

class WanderForce : public SteeringForce {
public:

	WanderForce() {}
	virtual ~WanderForce() {}

	virtual glm::vec3 getForce(Agent* entity) const;
};

// obstacles
struct Obstacle {

	enum eType {
		SPHERE,
		BOX,
	};

	int type;
	glm::vec3 center;

		float radius;
			glm::vec3 extents;

};

class ObstacleAvoidanceForce : public SteeringForce {
public:

	ObstacleAvoidanceForce() : m_feelerLength(1) {}
	virtual ~ObstacleAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	void addSphereObstacle(float x, float y, float z, float radius) {
		Obstacle o;
		o.type = Obstacle::SPHERE;
		o.center.x = x;
		o.center.y = y;
		o.center.z = z;
		o.radius = radius;
		m_obstacles.push_back(o);
	}
	void addBoxObstacle(float x, float y, float z, float w, float h, float d = 0) {
		Obstacle o;
		o.type = Obstacle::BOX;
		o.center.x = x;
		o.center.y = y;
		o.center.z = z;
		o.extents.x = w;
		o.extents.y = h;
		o.extents.z = d;
		m_obstacles.push_back(o);
	}

	void clearObstacles() { m_obstacles.clear();  }

	virtual glm::vec3 getForce(Agent* entity) const;

public:

	float m_feelerLength;

	std::vector<Obstacle> m_obstacles;
};

// FLOCKING FORCES

class SeparationForce : public SteeringForce {
public:

	SeparationForce() {}
	virtual ~SeparationForce() {}

	void setEntities(std::vector<Agent>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	std::vector<Agent>*	m_entities;
	float					m_radius;
};

class CohesionForce : public SteeringForce {
public:

	CohesionForce() {}
	virtual ~CohesionForce() {}

	void setEntities(std::vector<Agent>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	std::vector<Agent>*	m_entities;
	float					m_radius;
};

class AlignmentForce : public SteeringForce {
public:

	AlignmentForce() {}
	virtual ~AlignmentForce() {}

	void setEntities(std::vector<Agent>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	std::vector<Agent>*	m_entities;
	float						m_radius;
};

class FlowForce : public SteeringForce {
public:

	FlowForce() : m_flowField(nullptr) {}
	virtual ~FlowForce() {}

	void setField(glm::vec3* flowField, int rows, int cols, int depth, float cellSize) {
		m_flowField = flowField;
		m_rows = rows;
		m_cols = cols;
		m_depth = depth;
		m_cellSize = cellSize;
	}

	virtual glm::vec3 getForce(Agent* entity) const;

protected:

	glm::vec3* m_flowField;
	int m_rows, m_cols, m_depth;
	float m_cellSize;
};

} // namespace ai