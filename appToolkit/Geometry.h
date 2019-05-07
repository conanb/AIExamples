#pragma once

#include <vector>
#include <glm/glm.hpp>

// utilities
inline float min(float a, float b) { return a < b ? a : b; }
inline float max(float a, float b) { return a > b ? a : b; }

inline void swap(float& a, float& b) { float c = a; a = b; b = c; }

inline bool operator < (const glm::ivec2& a, const glm::ivec2& b) {
	return a.x < b.x || a.y < b.y;
}

inline bool operator < (const glm::ivec3& a, const glm::ivec3& b) {
	return a.x < b.x || a.y < b.y || a.z < b.z;
}

inline bool operator < (const glm::ivec4& a, const glm::ivec4& b) {
	return a.x < b.x || a.y < b.y || a.z < b.z || a.w < b.w;
}

inline bool operator < (const glm::vec2& a, const glm::vec2& b) {
	return a.x < b.x || a.y < b.y;
}

inline bool operator < (const glm::vec3& a, const glm::vec3& b) {
	return a.x < b.x || a.y < b.y || a.z < b.z;
}

inline bool operator < (const glm::vec4& a, const glm::vec4& b) {
	return a.x < b.x || a.y < b.y || a.z < b.z || a.w < b.w;
}

template <typename T>
inline bool operator >= (const T& a, const T& b) {
	return !(a < b);
}

inline float clamp(float t, float a, float b) { return max(a, min(b, t)); }

template <typename T>
inline T clamp(const T& t, const T& a, const T& b) {
	return max(a, min(b, t)); 
}

namespace app {

	// simple axis-aligned bounding box that stores min/max
	template <typename T>
	class AABB {
	public:

		AABB() {}
		AABB(const T& min, const T& max) : min(min), max(max) {}

		void fit(const T* points, unsigned int count) {
			// invalidate the extents
			min = points[0];
			max = points[0];

			// find min and max of the points
			for (unsigned int i = 1;
				i < count;
				++i, ++points) {
				min = ::min(min, *points);
				max = ::max(max, *points);
			}
		}

		void fit(const std::vector<T>& points) {
			// invalidate the extents
			min = points[0];
			max = points[0];

			// find min and max of the points
			for (auto& p : points) {
				min = ::min(min, p);
				max = ::max(max, p);
			}
		}

		T closestPoint(const T& p) const {
			return clamp(p, min, max);
		}

		bool overlaps(const T& p) const {
			// test for not overlapped as it exits faster
			return !(p.x < min.x || p.y < min.y ||
				p.x > max.x || p.y > max.y);
		}

		bool overlaps(const AABB & other) const {
			// test for not overlapped as it exits faster
			return !(max.x < other.min.x || max.y < other.min.y ||
				min.x > other.max.x || min.y > other.max.y);
		}

		T center() const {
			return (min + max) * 0.5f;
		}

		T extents() const {
			return abs(max - min) * 0.5f;
		}

		/*std::vector<Vector2> corners() const {
			std::vector<Vector2> corners(4);
			corners[0] = min;
			corners[1] = { min.x, max.y };
			corners[2] = max;
			corners[3] = { max.x, min.y };
			return corners;
		}*/

		T min, max;
	};

	// simple sphere
	template <typename T>
	class Sphere {
	public:

		Sphere() {}
		Sphere(const T& p, float r) : center(p), radius(r) {}

		void fit(const T* points, unsigned int count) {
			// invalidate extents
			T min = points[0];
			T max = points[0]

				// find min and max of the points
				for (unsigned int i = 0; i < count; ++i, ++points) {
					min = ::min(min, *points);
					max = ::max(max, *points);
				}

			// put a circle around the min/max box
			center = (min + max) * 0.5f;
			radius = center.distanceTo(max);
		}

		void fit(const std::vector<T> & points) {
			// invalidate extents
			T min = points[0];
			T max = points[0];

			// find min and max of the points
			for (auto& p : points) {
				min = ::min(min, p);
				max = ::max(max, p);
			}

			// put a circle around the min/max box
			center = (min + max) * 0.5f;
			radius = center.distanceTo(max);
		}

		T closestPoint(const T & p) const {
			// distance from center
			T toPoint = p - center;

			// if outside of radius bring it back to the radius
			if (glm::dot(toPoint, toPoint) > radius * radius) {
				toPoint = glm::normalize(toPoint) * radius;
			}

			return center + toPoint;
		}

		bool overlaps(const T& p) const {
			T toPoint = p - center;
			return glm::dot(toPoint, toPoint) <= (radius * radius);
		}

		bool overlaps(const Sphere<T>& other) const {
			T diff = other.center - center;
			// compare distance between spheres to combined radii
			float r = radius + other.radius;
			return glm::dot(diff, diff) <= (r * r);
		}

		bool overlaps(const AABB<T> & aabb) const {
			auto diff = center - aabb.closestPoint(center);
			return glm::dot(diff, diff) <= (radius * radius);
		}

		T center;
		float radius;
	};

	// 2D mathematical plane using Ax + By + d = 0, storing Normal and offset -d
	template <typename T>
	class Plane {
	public:

		Plane() {}
		Plane(const T& n, float d) : N(n), d(d) {}

		T N = {};
		float d = 0;

		/*Plane(const Vector2& p1, const Vector2& p2) {

			// calculate normalised vector from p0 to p1
			auto v = p2 - p1;
			v.normalise();

			// set normal perpendicular to the vector
			N.x = -v.y;
			N.y = v.x;

			// calculate d
			d = -p1.dot(N);
		}*/

		/*Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3) {

			// calculate edge vectors
			auto v1 = (p2 - p1).normalised();
			auto v2 = (p3 - p1).normalised();

			// calculate normal
			N = v1.cross(v2);

			// calculate d
			d = -p1.dot(N);
		}*/

		float distanceTo(const T& p) const {
			return glm::dot(p, N) + d;
		}

		T closestPoint(const T& p) const {
			return p - N * distanceTo(p);
		}

		enum ePlaneResult : int {
			FRONT = 1,
			BACK = -1,
			INTERSECTS = 0
		};

		ePlaneResult testSide(const T& p) const {
			float t = glm::dot(p, N) + d;

			if (t < 0)
				return ePlaneResult::BACK;
			else if (t > 0)
				return ePlaneResult::FRONT;

			return ePlaneResult::INTERSECTS;
		}

		ePlaneResult testSide(const Sphere<T>& sphere) const {
			float t = distanceTo(sphere.center);

			if (t > sphere.radius)
				return ePlaneResult::FRONT;
			else if (t < -sphere.radius)
				return ePlaneResult::BACK;

			return ePlaneResult::INTERSECTS;
		}

		ePlaneResult testSide(const AABB<T> & aabb) const {

			// tag if we find a corner on each side
			bool side[2] = { false, false };

			// compare each corner
			for (auto c : aabb.corners()) {
				auto result = testSide(c);
				if (result == ePlaneResult::FRONT)
					side[0] = true;
				else if (result == ePlaneResult::BACK)
					side[1] = true;
			}

			// if front but not back
			if (side[0] && !side[1])
				return ePlaneResult::FRONT;
			// if back but not front
			else if (!side[0] && side[1])
				return ePlaneResult::BACK;
			// else overlapping
			return ePlaneResult::INTERSECTS;
		}
	};

	// ray where direction should be normalised
	template <typename T>
	class Ray {
	public:

		Ray() {}

		// default infinite length
		Ray(const T& start, const T& dir, float l = INFINITY)
			: origin(start), direction(dir), length(l) {}

		T closestPoint(const T& point) const {
			// ray origin to arbitrary point
			auto p = point - origin;

			// project the point onto the ray and clamp by ray length
			float t = clamp(glm::dot(p, direction), 0, length);

			// return position in direction of ray
			return origin + direction * t;
		}

		bool intersects(const Sphere<T>& sphere,
			T* I = nullptr,
			T* R = nullptr) const {

			// ray origin to sphere center
			auto L = sphere.center - origin;

			// project sphere center onto ray
			float t = glm::dot(L, direction);

			// get sqr distance from sphere center to ray
			float dd = glm::dot(L, L) - t * t;

			// subtract penetration amount from projected distance
			t -= sqrt(sphere.radius * sphere.radius - dd);

			// it intersects if within ray length
			if (t >= 0 &&
				t <= length) {

				// store intersection point if requested
				if (I != nullptr)
					* I = origin + direction * t;

				if (R != nullptr) {
					// get surface normal at intersection point
					auto N = ((origin + direction * t) - sphere.center) / sphere.radius;
					// get penetration vector
					auto P = direction * (length - t);
					// get penetration amount
					float p = glm::dot(P, N);
					// get reflected vector
					*R = N * -2 * p + P;
				}

				return true;
			}

			// default no intersection
			return false;
		}

		bool intersects(const Plane<T> & plane,
			T * I = nullptr,
			T * R = nullptr) const {

			// project ray direction onto plane normal
			// this should give us a value between -1 and 1
			float t = glm::dot(direction, plane.N);

			// must face the plane
			if (t > 0)
				return false;

			// get distance of ray origin to the plane
			float d = glm::dot(origin, plane.N) + plane.d;

			// check if ray is parallel with the plane
			// no intersection if parallel and not touching
			if (t == 0 &&
				d != 0)
				return false;

			// calculate distance along ray to plane
			t = d == 0 ? 0 : -(d / t);

			// intersects if within range
			if (t >= 0 &&
				t <= length) {
				// store intersection point if requested
				if (I != nullptr)
					* I = origin + direction * t;

				if (R != nullptr) {
					// get penetration vector
					auto P = direction * (length - t);
					// get penetration amount
					float p = glm::dot(P, plane.N);
					// get reflected vector
					*R = plane.N * -2 * p + P;
				}

				return true;
			}

			return false;
		}

		/*bool intersects(const AABB& aabb,
						Vector2* I = nullptr,
						Vector2* R = nullptr) const {

			// get distances to each axis of the box
			float xmin, xmax, ymin, ymax;

			// get min and max in the x-axis
			if (direction.x < 0) {
				xmin = (aabb.max.x - origin.x) / direction.x;
				xmax = (aabb.min.x - origin.x) / direction.x;
			}
			else {
				xmin = (aabb.min.x - origin.x) / direction.x;
				xmax = (aabb.max.x - origin.x) / direction.x;
			}

			// get min and max in the y-axis
			if (direction.y < 0) {
				ymin = (aabb.max.y - origin.y) / direction.y;
				ymax = (aabb.min.y - origin.y) / direction.y;
			}
			else {
				ymin = (aabb.min.y - origin.y) / direction.y;
				ymax = (aabb.max.y - origin.y) / direction.y;
			}

			// ensure within box
			if (xmin > ymax ||
				ymin > xmax)
				return false;

			// the first contact is the largest of the two min
			float t = max(xmin, ymin);

			// intersects if within range
			if (t >= 0 &&
				t <= length) {

				// store intersection point if requested
				if (I != nullptr)
					*I = origin + direction * t;

				if (R != nullptr) {

					// need to determine box side hit
					Vector2 N = {};
					if (t == xmin) {
						// horizontal normal
						if (direction.x < 0)
							// right side
							N = { 1,0 };
						else
							// left side
							N = { -1,0 };
					}
					else {
						// vertical normal
						if (direction.y < 0)
							// top
							N = { 0,1 };
						else
							// bottom
							N = { 0,-1 };
					}

					// get penetration vector
					auto P = direction * (length - t);

					// get penetration amount
					float p = P.dot(N);

					// get reflected vector
					*R = N * -2 * p + P;
				}

				return true;
			}

			// not within Ray's range
			return false;
		}*/

		/*bool intersects(const Ray<T>& ray,
						T* I = nullptr,
						T* R = nullptr) const {

			// METHOD 1 - solve algebraically
			// P is intersection point, u and v are length along rays to intersection point
			// oN is ray origin, dN is ray direction, uses infinite length to find intersection
			// P = o1 + d1 * u
			// P = o2 + d2 * v
			// Px = o1x + d1x * u
			// Py = o1y + d1y * u
			// Px = o2x + d2x * v
			// Py = o2y + d2y * v

			// o1x + d1x * u = o2x + d2x * v
			// o1y + d1y * u = o2y + d2y * v

			// solve for v
			// subtract o2x from both sides
			// o1x + d1x * u - o2x = o2x + d2x * v - o2x
			// o1x + d1x * u - o2x = d2x * v
			// divide both sides by d2x
			// (o1x + d1x * u - o2x) / d2x = d2x * v / d2x
			// (o1x + d1x * u - o2x) / d2x = v

			// substitute v
			// o1y + d1y * u = o2y + d2y * v
			// o1y + d1y * u = o2y + d2y * ((o1x + d1x * u - o2x) / d2x)

			// solve for u
			// u = (o1y * d2x + d2y * o2x - o2y * d2x - o1x * d2y) / (d1x * d2y - d1y * d2x)

			// find u first
			auto u = (origin.y * ray.direction.x + ray.direction.y * ray.origin.x - ray.origin.y * ray.direction.x - origin.x * ray.direction.y);
			u /= direction.x * ray.direction.y - direction.y * ray.direction.x;

			// now find v
			auto v = (origin.x + direction.x * u - ray.origin.x) / ray.direction.x;

			// METHOD 2 - reduced ray vs plane intersections
			// u = ((o2y - o1y) * d2x - (o2x - o1x) * d2y) / (d2x * d1y - d2y * d1x)
			// v = ((o2y - o1y) * d1x - (o2x - o1x) * d1y) / (d2x * d1y - d2y * d1x)
			// reduce common terms
			// dx = o2x - o1x
			// dy = o2y - o1y
			// det = d2x * d1y - d2y * d1x
			// substitute common terms
			// u = (dy * d2x - dx * d2y) / det
			// v = (dy * d1x - dx * d1y) / det

			// separating axis
			auto dx = ray.origin.x - origin.x;
			auto dy = ray.origin.y - origin.y;

			// relative distance to intersection
			// one ray direction is made perpendicular to act as a plane normal
			auto det = direction.y * ray.direction.x - direction.x * ray.direction.y;

			// reduced ray/plane distance checks for each ray
			u = (dy * ray.direction.x - dx * ray.direction.y) / det;
			v = (dy * direction.x - dx * direction.y) / det;

			// test for intersection
			if (det != 0 &&
				u >= 0 && u <= length &&
				v >= 0 && v <= ray.length) {

				if (I != nullptr)
					*I = origin + direction * u;

				if (R != nullptr) {
					// convert ray into plane normal
					Vector2 N = { ray.direction.y, -ray.direction.x };
					// get penetration vector
					auto P = direction * (length - u);
					// get penetration amount
					float p = P.dot(N);
					// get reflected vector
					*R = N * -2 * p + P;
				}

				return true;
			}

			return false;
		}*/

		T origin;
		T direction;
		float length;
	};

} // namespace app