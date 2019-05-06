#pragma once

#include <random>
#include "Vector.h"

class Random {
public:
	// set starting ranges
	Random() : m_intParams(0, INT_MAX), m_floatParams(0.0f, 1.0f) {}

	void seed(unsigned int value) {
		m_engine.seed(value);
	}

	int next() {
		m_intParams._Min = 0;
		m_intParams._Max = INT_MAX;
		m_intDistribution.param(m_intParams);
		return m_intDistribution(m_engine);
	}

	float nextReal() {
		m_floatParams._Min = 0.0f;
		m_floatParams._Max = 1.0f;
		m_floatDistribution.param(m_floatParams);
		return m_floatDistribution(m_engine);
	}

	int range(int min, int max) {
		// set distribution range
		m_intParams._Min = min;
		m_intParams._Max = max;
		m_intDistribution.param(m_intParams);
		// generate value
		return m_intDistribution(m_engine);
	}

	float range(float min, float max) {
		// set distribution range
		m_floatParams._Min = min;
		m_floatParams._Max = max;
		m_floatDistribution.param(m_floatParams);
		// generate value
		return m_floatDistribution(m_engine);
	}

	Vector2 unitCircle() {
		// set distribution range
		m_floatParams._Min = 0;
		m_floatParams._Max = 2 * 3.14159f;
		m_floatDistribution.param(m_floatParams);
		float angle = m_floatDistribution(m_engine);
		return{ cosf(angle), sinf(angle) };
	}

	Vector3 unitSphere() {
		// set distribution range
		m_floatParams._Min = 0;
		m_floatParams._Max = 2 * 3.14159f;
		m_floatDistribution.param(m_floatParams);
		float longitude = m_floatDistribution(m_engine);
		float latitude = m_floatDistribution(m_engine);

		return{ sinf(latitude) * sinf(longitude),
			cosf(latitude),
			sinf(latitude) * cosf(longitude)
		};
	}

private:

	std::default_random_engine m_engine;

	std::uniform_int_distribution<int>::param_type m_intParams;
	std::uniform_real_distribution<float>::param_type m_floatParams;

	std::uniform_int_distribution<int> m_intDistribution;
	std::uniform_real_distribution<float> m_floatDistribution;
};