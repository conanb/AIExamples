#include "aiUtilities.h"

#include <math.h>
#include <glm/ext.hpp>

// returns true if the ray intersects the circle
bool rayCircleIntersection(float px, float py,	// ray start
						   float dx, float dy,	// ray direction
						   float cx, float cy, float r,	// circle position and radius
						   float& ix, float& iy,	// intersection point
						   float* t) {	// distance along normalised ray direction to intersection

													// normalise direction
	float temp = dx * dx + dy * dy;
	if (temp == 0)
		return false;

	temp = sqrtf(temp);

	dx /= temp;
	dy /= temp;

	// get vector from line start to circle centre
	float ex = cx - px;
	float ey = cy - py;

	// get squared length of e
	float e2 = ex * ex + ey * ey;

	// get squared radius
	float r2 = r * r;

	// determine if starting inside circle
	if (e2 < r2) {
		// if inside then reverse test direction
		dx *= -1;
		dy *= -1;
	}

	// project sphere centre onto d to get edge of a triangle
	float a = ex * dx + ey * dy;

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

	ix = dx * temp + px;
	iy = dy * temp + py;

	if (t != nullptr)
		*t = temp;

	return true;
}

bool rayBoxIntersection(float px, float py,	// ray start
						float dx, float dy,	// ray direction
						float x, float y, float w, float h,	// box position and size
						float& nx, float& ny,	// normal of intersection
						float* t) {	// distance along ray direction to intersection

	using glm::max;
	using glm::min;

	bool inside = true;

	nx = ny = 0;

	float xt;
	if (px < x) {
		xt = x - px;
		if (xt > dx)
			return false;
		inside = false;
		xt /= dx;
		nx = -1;
	}
	else if (px > (x + w)) {
		xt = (x + w) - px;
		if (xt < dx)
			return false;
		inside = false;
		xt /= dx;
		nx = 1;
	}
	else {
		xt = -1;
	}

	float yt;
	if (py < y) {
		yt = y - py;
		if (yt > dy)
			return false;
		inside = false;
		yt /= dy;
		ny = -1;
	}
	else if (py > (y + h)) {
		yt = (y + h) - py;
		if (yt < dy)
			return false;
		inside = false;
		yt /= dy;
		ny = 1;
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
		float ty = py + dy * yt;
		if (ty < y || ty > (y + h))
			return false;
		if (t != nullptr)
			*t = yt;
	}
	else {
		// intersect with x plane?
		float tx = px + dx * xt;
		if (tx < x || tx > (x + w))
			return false;
		if (t != nullptr)
			*t = xt;
	}
	
	return true;
}