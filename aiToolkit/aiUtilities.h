#pragma once

// returns true if the ray intersects the circle
bool rayCircleIntersection(float px, float py,	// ray start
						   float dx, float dy,	// ray direction
						   float cx, float cy, float r,	// circle position and radius
						   float& ix, float& iy,	// intersection point
						   float* t = nullptr);	// distance along normalised ray direction to intersection

// returns true if the ray intersects the circle
bool rayBoxIntersection(float px, float py,	// ray start
						float dx, float dy,	// ray direction (length of ray included)
						float x, float y, float w, float h, // box position and size
						float& nx, float& ny,	// normal of intersection
						float* t = nullptr); // distance along ray direction to intersection