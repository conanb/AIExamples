#include "Intersection.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <math.h>

namespace intersection {

// returns true if the ray intersects the circle
bool rayCircleIntersection(const glm::vec3& p,	// ray start
						   const glm::vec3& d,	// ray direction
						   const glm::vec3& c, float r,	// circle position and radius
						   glm::vec3& i,	// intersection point
						   float* t) {	// distance along normalised ray direction to intersection

	// normalise direction
	float temp = glm::dot(d,d);
	if (temp == 0)
		return false;

	auto dir = glm::normalize(d);

	// get vector from line start to circle centre
	auto e = c - p;

	// get squared length of e
	float e2 = glm::dot(e,e);

	// get squared radius
	float r2 = r * r;

	// determine if starting inside circle
	if (e2 < r2) {
		// if inside then reverse test direction
		dir *= -1;
	}

	// project sphere centre onto d to get edge of a triangle
	float a = glm::dot(e, dir);

	// squared edge length
	float a2 = a * a;

	// use pythagoras to determine intersection
	float f2 = r2 - e2 + a2;
	// f2 is now the amount of penetration into the circle squared

	// if negative then no penetration
	if (f2 < 0)
		return false;

	// calculate distance in direction d from p that the intersection occurs
	temp = a - sqrtf(f2);

	i = dir * temp + p;

	if (t != nullptr)
		*t = temp;

	return true;
}

bool rayBoxIntersection(const glm::vec3& p,	// ray start
						const glm::vec3& d,	// ray direction (length of ray included)
						const glm::vec3& c, const glm::vec3& e, // box position and size
						glm::vec3& n,	// normal of intersection
						float* t) {	// distance along ray direction to intersection

	using glm::max;
	using glm::min;

	bool inside = true;

	n = { 0,0,0 };

	float xt;
	if (p.x < c.x) {
		xt = c.x - p.x;
		if (xt > d.x)
			return false;
		inside = false;
		xt /= d.x;
		n.x = -1;
	}
	else if (p.x > (c.x + e.x)) {
		xt = (c.x + e.x) - p.x;
		if (xt < d.x)
			return false;
		inside = false;
		xt /= d.x;
		n.x = 1;
	}
	else {
		xt = -1;
	}

	float yt;
	if (p.y < c.y) {
		yt = c.y - p.y;
		if (yt > d.y)
			return false;
		inside = false;
		yt /= d.y;
		n.y = -1;
	}
	else if (p.y > (c.y + e.y)) {
		yt = (c.y + e.y) - p.y;
		if (yt < d.y)
			return false;
		inside = false;
		yt /= d.y;
		n.y = 1;
	}
	else {
		yt = -1;
	}

	if (inside) {
		if (t != nullptr)
			*t = 0;
	}
	else if (yt > xt) {
		// intersect with y plane?
		float ty = p.y + d.y * yt;
		if (ty < c.y || ty > (c.y + e.y))
			return false;
		if (t != nullptr)
			*t = yt;
	}
	else {
		// intersect with x plane?
		float tx = p.x + d.x * xt;
		if (tx < c.x || tx > (c.x + e.x))
			return false;
		if (t != nullptr)
			*t = xt;
	}
	
	return true;
}
}