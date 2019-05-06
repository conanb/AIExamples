#pragma once

#include <math.h>

namespace tween {

	template <typename T>
	inline T easeLinear(float t, const T& b, const T& c, float d) {
		return b + c * (t / d);
	}

	template <typename T>
	static T easeOutSine(float t, const T& b, const T& c, float d) {
		return b + c * sinf(t / d * 3.14159265f / 2);
	}

	template <typename T>
	inline T easeInSine(float t, const T& b, const T& c, float d) {
		return b + c - c * cosf(t / d * 3.14159f);
	}

	template <typename T>
	static T easeInOutSine(float t, const T& b, const T& c, float d) {
		return b - c / 2 * (cosf(3.14159265f * t / d) - 1);
	}

	template <typename T>
	static T easeOutBounce(float t, const T& b, const T& c, float d) {
		if ((t /= d) < (1 / 2.75f)) {
			return c*(7.5625f*t*t) + b;
		}
		else if (t < (2 / 2.75f)) {
			float postFix = t -= (1.5f / 2.75f);
			return c*(7.5625f*(postFix)*t + .75f) + b;
		}
		else if (t < (2.5 / 2.75)) {
			float postFix = t -= (2.25f / 2.75f);
			return c*(7.5625f*(postFix)*t + .9375f) + b;
		}
		else {
			float postFix = t -= (2.625f / 2.75f);
			return c*(7.5625f*(postFix)*t + .984375f) + b;
		}
	}

	template <typename T>
	static T easeOutElastic(float t, const T& b, const T& c, float d) {
		float p = d * .3f;
		float a = c;
		float s = p / 4;
		return (a*powf(2, -10 * t) * sinf((t*d - s)*(2 * 3.14159265f) / p) + c + b);
	}
}