#pragma once

#include "Entity.h"

namespace ai {

// condition is also a behaviour so that it can work within a behaviour tree
class Condition : public Behaviour {
public:

	Condition() {}
	virtual ~Condition() {}

	virtual bool test(Entity* entity) const = 0;

	virtual eBehaviourResult execute(Entity* entity) {
		if (test(entity))
			return eBehaviourResult::SUCCESS;
		return eBehaviourResult::FAILURE;
	}
};

class FloatRangeCondition : public Condition {
public:

	FloatRangeCondition(const float* value, float min, float max)
		: m_value(value), m_min(min), m_max(max) {
	}
	virtual ~FloatRangeCondition() {}

	virtual bool test(Entity* entity) const {
		return (m_min <= *m_value) && (m_max >= *m_value);
	}

private:

	const float*	m_value;
	float			m_min, m_max;
};

class FloatGreaterCondition : public Condition {
public:

	FloatGreaterCondition(const float* value, float compare)
		: m_value(value), m_compare(compare) {
	}
	virtual ~FloatGreaterCondition() {}

	virtual bool test(Entity* entity) const {
		return *m_value > m_compare;
	}

private:

	const float*	m_value;
	float			m_compare;
};

class WithinRangeCondition : public Condition {
public:

	WithinRangeCondition(const Entity* target, float range)
		: m_target(target), m_range(range) {}
	virtual ~WithinRangeCondition() {}

	virtual bool test(Entity* entity) const {
		// get target position
		auto target = m_target->getPosition();

		// get my position
		auto position = entity->getPosition();

		// compare the two
		return glm::distance(target, position) <= m_range;
	}

private:

	const Entity* m_target;
	float m_range;
};

class NotCondition : public Condition {
public:

	NotCondition(const Condition* condition) : m_condition(condition) {}
	virtual ~NotCondition() {}

	virtual bool test(Entity* entity) const {
		return !m_condition->test(entity);
	}

private:

	const Condition* m_condition;
};

} // namespace ai