#pragma once

#include <glm/fwd.hpp>

namespace intersection {

	// returns true if the ray intersects the circle
	bool rayCircleIntersection(const glm::vec3& p,	// ray start
							   const glm::vec3& d,	// ray direction
							   const glm::vec3& c, float r,	// circle position and radius
							   glm::vec3& i,	// intersection point
							   float* t = nullptr);	// distance along normalised ray direction to intersection

	// returns true if the ray intersects the circle
	bool rayBoxIntersection(const glm::vec3& p,	// ray start
							const glm::vec3& d,	// ray direction (length of ray included)
							const glm::vec3& c, const glm::vec3& e, // box position and size
							glm::vec3& n,	// normal of intersection
							float* t = nullptr); // distance along ray direction to intersection

} // namespace intersection