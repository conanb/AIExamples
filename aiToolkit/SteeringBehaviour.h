#pragma once

#include "State.h"

// could be replaced with a vector2
struct Force {
	float x, y;
};

struct WanderData {
	float offset;
	float radius;
	float jitter;
	float x, y;
};

// abstract class
class SteeringForce {
public:

	SteeringForce() {}
	virtual ~SteeringForce() {}

	// pure virtual function
	virtual Force getForce(GameObject* gameObject) const = 0;
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

	virtual eBehaviourResult execute(GameObject* gameObject, float deltaTime);

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

	virtual void update(GameObject* gameObject, float deltaTime);

protected:

	std::vector<WeightedForce>	m_forces;
};

class SeekForce : public SteeringForce {
public:

	SeekForce(GameObject* target = nullptr) : m_target(target) {}
	virtual ~SeekForce() {}

	void setTarget(GameObject* target) { m_target = target; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject*	m_target;
};

class FleeForce : public SteeringForce {
public:

	FleeForce(GameObject* target = nullptr) : m_target(target) {}
	virtual ~FleeForce() {}

	void setTarget(GameObject* target) { m_target = target; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject*	m_target;
};

class PursueForce : public SteeringForce {
public:

	PursueForce(GameObject* target = nullptr) : m_target(target) {}
	virtual ~PursueForce() {}

	void setTarget(GameObject* target) { m_target = target; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject*	m_target;
};

class EvadeForce : public SteeringForce {
public:

	EvadeForce(GameObject* target = nullptr) : m_target(target) {}
	virtual ~EvadeForce() {}

	void setTarget(GameObject* target) { m_target = target; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	GameObject*	m_target;
};

class WanderForce : public SteeringForce {
public:

	WanderForce() {}
	virtual ~WanderForce() {}

	virtual Force getForce(GameObject* gameObject) const;

};

// obstacles
struct Obstacle {

	enum eType {
		SPHERE,
		BOX,
	};

	int type;
	float x, y;
	union {
		float r;
		struct {
			float w, h;
		};
	};
};

class ObstacleAvoidanceForce : public SteeringForce {
public:

	ObstacleAvoidanceForce() : m_feelerLength(1) {}
	virtual ~ObstacleAvoidanceForce() {}

	void setFeelerLength(float length) { m_feelerLength = length; }

	void addSphereObstacle(float x, float y, float radius) {
		Obstacle o = { Obstacle::SPHERE, x, y };
		o.r = radius;
		m_obstacles.push_back(o);
	}
	void addBoxObstacle(float x, float y, float w, float h) {
		Obstacle o = { Obstacle::BOX, x, y };
		o.w = w;
		o.h = h;
		m_obstacles.push_back(o);
	}

	void clearObstacles() { m_obstacles.clear();  }

	virtual Force getForce(GameObject* gameObject) const;

public:

	float m_feelerLength;

	std::vector<Obstacle> m_obstacles;
};

// FLOCKING FORCES

class SeparationForce : public SteeringForce {
public:

	SeparationForce() {}
	virtual ~SeparationForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};

class CohesionForce : public SteeringForce {
public:

	CohesionForce() {}
	virtual ~CohesionForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};

class AlignmentForce : public SteeringForce {
public:

	AlignmentForce() {}
	virtual ~AlignmentForce() {}

	void setEntities(std::vector<GameObject>* entities) { m_entities = entities; }
	void setRadius(float radius) { m_radius = radius; }

	virtual Force getForce(GameObject* gameObject) const;

protected:

	std::vector<GameObject>*	m_entities;
	float						m_radius;
};

class FlowForce : public SteeringForce {
public:

	FlowForce() : m_flowField(nullptr) {}
	virtual ~FlowForce() {}

	void setField(Vector2* flowField, int rows, int cols, float cellSize) {
		m_flowField = flowField;
		m_rows = rows;
		m_cols = cols;
		m_cellSize = cellSize;
	}

	virtual Force getForce(GameObject* gameObject) const;

protected:

	Vector2* m_flowField;
	int m_rows, m_cols;
	float m_cellSize;
};