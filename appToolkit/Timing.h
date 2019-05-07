#pragma once

#include <chrono>

#include <map>
#include <vector>
#include <stack>
#include <cassert>

namespace app {

	class Time {

		typedef std::chrono::high_resolution_clock Clock;

	public:

		static void initialise() {
			assert(m_singleton == nullptr);
			m_singleton = new Time();
		}
		static void tick() {
			auto now = Clock::now();

			m_singleton->m_deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_singleton->m_lastTick).count() / 1000.f;
			m_singleton->m_now = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_singleton->m_epoch).count() / 1000.f;
			m_singleton->m_lastTick = now;
			m_singleton->m_frameCount++;
		}

		static void reset() {
			m_singleton->m_epoch = m_singleton->m_lastTick = Clock::now();
			m_singleton->m_deltaTime = 0;
			m_singleton->m_now = 0;
			m_singleton->m_frameCount = 0;
		}

		static void shutdown() {
			delete m_singleton;
			m_singleton = nullptr;
		}

		static inline float deltaTime() { return m_singleton->m_deltaTime; }

		static inline float now() { return m_singleton->m_now; }

		static inline unsigned int frameCount() { return m_singleton->m_frameCount; }

	private:

		friend class Application;

		Time() {
			m_epoch = m_lastTick = Clock::now();
			m_deltaTime = 0;
			m_now = 0;
			m_frameCount = 0;
		}

		float m_now, m_deltaTime;
		unsigned int m_frameCount;

		Clock::time_point m_epoch;
		Clock::time_point m_lastTick;

		static Time* m_singleton;
	};

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

} // namespace app