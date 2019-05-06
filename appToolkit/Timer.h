#pragma once

#include <chrono>

#include <map>
#include <vector>
#include <stack>
#include <cassert>

// simple timer class
class Timer {

	typedef std::chrono::high_resolution_clock Clock;

public:

	Timer() {
		reset();
	}

	void reset() {
		m_epoch = Clock::now();
	}

	template <typename T>
	T elapsed() const {
		return std::chrono::duration_cast<T>(Clock::now() - m_epoch);
	}

	double seconds() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - m_epoch).count() / 1000.0;
	}

	long long milliseconds() const {
		return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - m_epoch).count();
	}

	long long nanoseconds() const {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - m_epoch).count();
	}

private:

	Clock::time_point m_epoch;

};