#pragma once

#include "Behaviour.h"
#include <iostream>

namespace ai {

// collection of sub-behaviours
class CompositeBehaviour : public Behaviour {
public:

	CompositeBehaviour() {
		m_runningBehaviour = m_children.end();
	}
	virtual ~CompositeBehaviour() {}

	void addChild(Behaviour* child) { m_children.push_back(child); m_runningBehaviour = m_children.end(); }
	unsigned int getChildCount() const { return m_children.size(); }

protected:

	std::vector<Behaviour*>	m_children;
	std::vector<Behaviour*>::iterator m_runningBehaviour;
};

// all children must succeed for it to succeed
class SequenceBehaviour : public CompositeBehaviour {
public:

	SequenceBehaviour() {}
	virtual ~SequenceBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {

		auto iter = m_runningBehaviour;

		m_runningBehaviour = m_children.end();

		if (iter == m_children.end())
			iter = m_children.begin();
		
		while (iter != m_children.end()) {

			auto result = (*iter)->execute(entity, deltaTime);

			if (result == eBehaviourResult::FAILURE)
				return eBehaviourResult::FAILURE;
			else if (result == eBehaviourResult::RUNNING) {
				m_runningBehaviour = iter;
				return eBehaviourResult::RUNNING;
			}

			iter++;
		}
		
		return eBehaviourResult::SUCCESS;
	}
};

// when first child succeeds it succeeds
class SelectorBehaviour : public CompositeBehaviour {
public:

	SelectorBehaviour() {}
	virtual ~SelectorBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {

		auto iter = m_runningBehaviour;

		m_runningBehaviour = m_children.end();

		if (iter == m_children.end())
			iter = m_children.begin();

		while (iter != m_children.end()) {

			auto result = (*iter)->execute(entity, deltaTime);

			if (result == eBehaviourResult::SUCCESS)
				return eBehaviourResult::SUCCESS;
			else if (result == eBehaviourResult::RUNNING) {
				m_runningBehaviour = iter;
				return eBehaviourResult::RUNNING;
			}

			iter++;
		}

		return eBehaviourResult::FAILURE;
	}
};

class SwitchBehaviour : public CompositeBehaviour {
public:

	SwitchBehaviour() {}
	virtual ~SwitchBehaviour() {}

	void setIndex(unsigned int index) { m_index = index; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {
		if (m_index < 0 ||
			m_index >= m_children.size())
			return eBehaviourResult::FAILURE;

		return m_children[m_index]->execute(entity, deltaTime);
	}

protected:

	unsigned int m_index = 0;
};

class RandomBehaviour : public CompositeBehaviour {
public:

	RandomBehaviour() {}
	virtual ~RandomBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {

		if (m_children.empty())
			return eBehaviourResult::FAILURE;

		return m_children[rand() % m_children.size()]->execute(entity, deltaTime);
	}
};

class RandomSelectorBehaviour : public SelectorBehaviour {
public:

	RandomSelectorBehaviour() {}
	virtual ~RandomSelectorBehaviour() {}

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {

		// Shuffle all child nodes!
		std::vector<Behaviour*> newList;
		while (m_children.empty() == false) {

			// randomly pick one out of the old list
			// remove it from old list
			// put in new list
		}

		m_children = newList;

		// once shuffled, run as a normal selector
		return SelectorBehaviour::execute(entity, deltaTime);
	}
};

class NotDecorator : public Behaviour {
public:

	NotDecorator(Behaviour* child = nullptr) : m_child(child) {}
	virtual ~NotDecorator() {}

	void setChild(Behaviour* child) { m_child = child; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {

		if (m_child != nullptr) {

			auto r = m_child->execute(entity, deltaTime);

			switch (r) {
			case eBehaviourResult::SUCCESS:	return eBehaviourResult::FAILURE;
			case eBehaviourResult::FAILURE: return eBehaviourResult::SUCCESS;
			default: return eBehaviourResult::RUNNING;
			};
		}
		return eBehaviourResult::FAILURE;
	}

protected:

	Behaviour*	m_child;
};

class LogDecorator : public Behaviour {
public:

	LogDecorator(Behaviour* child = nullptr, const char* msg = "") : m_child(child), m_message(msg) {}
	virtual ~LogDecorator() {}

	void setChild(Behaviour* child) { m_child = child; }
	void setMessage(const char* msg) { m_message = msg; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {
		if (m_child != nullptr) {

			std::cout << m_message << std::endl;

			return m_child->execute(entity, deltaTime);
		}
		return eBehaviourResult::FAILURE;
	}

protected:

	std::string	m_message;
	Behaviour*	m_child;
};

class LimitDecorator : public Behaviour {
public:

	LimitDecorator(Behaviour* child, int limit = 1) : m_child(child), m_count(limit) {}
	virtual ~LimitDecorator() {}

	void setLimit(int limit) { m_count = limit; }
	void setChild(Behaviour* child) { m_child = child; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime) {
		if (m_child != nullptr &&
			m_count > 0) {

			--m_count;

			return m_child->execute(entity, deltaTime);
		}
		return eBehaviourResult::FAILURE;
	}

protected:

	int			m_count;
	Behaviour*	m_child;
};

class TimeoutDecorator : public Behaviour {
public:

	TimeoutDecorator(Behaviour* child, float cooldown = 0) : m_child(child), m_cooldown(cooldown) {}
	virtual ~TimeoutDecorator() {}

	void setCooldown(float cooldown) { m_cooldown = cooldown; }
	void setChild(Behaviour* child) { m_child = child; }

	virtual eBehaviourResult execute(Entity* entity, float deltaTime);

protected:

	float		m_lastTime = -1;
	float		m_cooldown;
	Behaviour*	m_child;
};

} // namespace ai