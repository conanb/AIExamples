#pragma once

#include "Curve.h"
#include <cassert>

template <typename T>
class CatmullRomSpline {
public:

	CatmullRomSpline() {}
	CatmullRomSpline(const std::vector<T>& points) {
		build(points);
	}

	struct ControlPoint {
		T point, tangent;
	};

	typedef std::vector<ControlPoint> ControlPointArray;

	const ControlPointArray& getControlPoints() const { return m_controlPoints; }

	void build(const std::vector<T>& points) {

		// ensure valid points
		assert(points.size() >= 2);

		// clear and resize control point array
		m_controlPoints.clear();		
		m_controlPoints.resize(points.size());

		// set control points and calculate tangents
		for (int i = 0; i < points.size(); ++i) {
			// copy point
			m_controlPoints[i].point = points[i];

			// calculate tangent
			if (i == 0) {
				// first
				m_controlPoints[i].tangent = points[i + 1] - points[i];
			}
			else if (i == points.size() - 1) {
				// last
				m_controlPoints[i].tangent = points[i] - points[i - 1];
			}
			else {
				m_controlPoints[i].tangent = points[i + 1] - points[i - 1];
			}
		}
	}

	T evaluate(float t) const {

		assert(!m_controlPoints.empty());

		// early out
		if (t <= 0)
			return m_controlPoints.front().point;
		if (t >= 1)
			return m_controlPoints.back().point;

		// how long does a single curve segment last for
		float step = 1.0f / (m_controlPoints.size()-1);

		// how many segments have passed
		int p0 = int(t / step);
		int p1 = p0 + 1;

		// how far are we through this segment
		step = fmod(t, step) / step;

		// calculate tangents
		T t0, t1;
		if (p0 == 0) {
			t0 = m_controlPoints[p0 + 1].point - m_controlPoints[p0].point;
		}
		else if (p0 == m_controlPoints.size() - 1) {
			t0 = m_controlPoints[p0 + 1].point - m_controlPoints[p0].point;
		}
		else {
			t0 = m_controlPoints[p0 + 1].point - m_controlPoints[p0 - 1].point;
		}

		if (p1 == 0) {
			t1 = m_controlPoints[p1 + 1].point - m_controlPoints[p1].point;
		}
		else if (p1 == m_controlPoints.size() - 1) {
			t1 = m_controlPoints[p1 + 1].point - m_controlPoints[p1].point;
		}
		else {
			t1 = m_controlPoints[p1 + 1].point - m_controlPoints[p1 - 1].point;
		}

		return Curve::hermite(m_controlPoints[p0].point, t0,
							  m_controlPoints[p1].point, t1,
							  step);
	}

private:

	ControlPointArray m_controlPoints;
};
