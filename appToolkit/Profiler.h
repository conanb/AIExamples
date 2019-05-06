#pragma once

#include <chrono>

#include <stack>
#include <cassert>

class Profiler {
	typedef std::chrono::high_resolution_clock Clock;
	typedef Clock::time_point TimePoint;

public:

	static Profiler* create(unsigned int count) {
		assert(m_singleton == nullptr);
		m_singleton = new Profiler(count);
		return m_singleton;
	}
	static void destroy() {
		delete m_singleton;
		m_singleton = nullptr;
	}
	static Profiler* getInstance() { return m_singleton; }

	const long long* getTimes() const { return m_timers; }

	void push(unsigned int id) {
		assert(id < m_count);
		m_pushedTimes.push(std::make_pair(id, Clock::now()));
	}

	void pop() {
		auto top = m_pushedTimes.top();
		m_timers[top.first] += std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - top.second).count();
		m_pushedTimes.pop();
	}

	void reset() {
		memset(m_timers, 0, sizeof(long long) * m_count);
	}

private:

	Profiler(unsigned int count) : m_count(count), m_timers(new long long[count]) { reset(); }
	~Profiler() { delete[] m_timers; }

	unsigned int m_count;
	long long* m_timers;

	std::stack<std::pair<unsigned int, TimePoint>> m_pushedTimes;

	static Profiler* m_singleton;
};

class ProfileStep {
public:

	ProfileStep(unsigned int id) {
		Profiler::getInstance()->push(id);
	}
	~ProfileStep() {
		Profiler::getInstance()->pop();
	}
};