#include "SteeringBehaviour.h"
#include "Intersection.h"
#include "Blackboard.h"
#include "Timing.h"
#include "Geometry.h"
#include <glm/ext.hpp>

namespace ai {

eBehaviourResult SteeringBehaviour::execute(Entity* entity) {

	glm::vec3 force(0);

	glm::vec3* velocity = nullptr;
	if (entity->getBlackboard().get("velocity", &velocity) == false)
		return eBehaviourResult::FAILURE;

	float maxVelocity = 0;
	if (entity->getBlackboard().get("maxVelocity", maxVelocity) == false)
		return eBehaviourResult::FAILURE;

	// accumulate forces
	for (auto& wf : m_forces)
		force += wf.force->getForce(entity) * wf.weight;

	*velocity += force * app::Time::deltaTime();

	// cap velocity (MOVE TO A VELOCITY BEHAVIOUR)
	float magnitudeSqr = glm::dot(*velocity, *velocity);
	if (magnitudeSqr > (maxVelocity * maxVelocity)) {
		*velocity /= sqrt(magnitudeSqr) * maxVelocity;
	}

	entity->translate(*velocity * app::Time::deltaTime());

	return eBehaviourResult::SUCCESS;
}

void SteeringState::update(Entity* entity) {

	glm::vec3 force(0);

	glm::vec3* velocity = nullptr;
	// must have velocity
	if (entity->getBlackboard().get("velocity", &velocity) == false)
		return;

	for (auto& wf : m_forces) {
		auto temp = wf.force->getForce(entity);

		// accumulate forces
		force += temp * wf.weight;
	}

	float maxVelocity = 0;
	entity->getBlackboard().get("maxVelocity", maxVelocity);

	*velocity += force * app::Time::deltaTime();

	// cap velocity
	float magnitudeSqr = glm::dot(*velocity, *velocity);
	if (magnitudeSqr > (maxVelocity * maxVelocity)) {
		float magnitude = sqrt(magnitudeSqr);
		*velocity /= magnitude * maxVelocity;
	}

	entity->translate(*velocity * app::Time::deltaTime());
}

glm::vec3 SeekForce::getForce(Entity* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get my position
	auto position = entity->getPosition();

	// get a vector to the target from "us"
	auto diff = target - position;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 0)
		diff = glm::normalize(diff);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return diff * maxForce;
}

glm::vec3 FleeForce::getForce(Entity* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = target - position;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 0)
		diff = glm::normalize(diff);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return diff * maxForce;
}

glm::vec3 PursueForce::getForce(Entity* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get target's velocity
	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	// add velocity to target
	target += *velocity;

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = target - position;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 0) 
		diff = glm::normalize(diff);

	return diff * maxForce;
}

glm::vec3 EvadeForce::getForce(Entity* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get target's velocity
	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);

	// add velocity to target
	target += *velocity;

	// get my position
	auto position = entity->getPosition();

	// compare the two and get the distance between them
	auto diff = position - target;

	// if not at the target then move towards them
	if (glm::dot(diff, diff) > 0)
		diff = glm::normalize(diff);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return diff * maxForce;
}

glm::vec3 WanderForce::getForce(Entity* entity) const {

	WanderData* wd = nullptr;
	if (entity->getBlackboard().get("wanderData", &wd) == false) {
		return glm::vec3(0);
	}

	// generate a random circular direction with a radius of "jitter"
	auto jitterOffset = glm::sphericalRand(wd->jitter) * wd->axisWeights;

	auto wander = wd->target;

	// apply the jitter to our current wander target
	wander += jitterOffset;

	// bring it back to a radius around the game object
	wander = glm::normalize(wander) * wd->radius;

	// store the target back into the game object
	wd->target = wander;

	// access the game object's velocity as a unit vector (normalised)
	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);
	float vx = velocity->x;
	float vy = velocity->y;

	// normalise and protect from divide-by-zero
	if (glm::dot(*velocity, *velocity) > 0)
		*velocity = glm::normalize(*velocity);
	
	// combine velocity direction with wander target to offset
	wander += *velocity * wd->offset;

	// normalise the new direction
	if (glm::dot(wander, wander) > 0)
		wander = glm::normalize(wander);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return wander * maxForce;
}

/*glm::vec3 ObstacleAvoidanceForce::getForce(Entity* entity) const {

	glm::vec3 force(0);

	// create feeler
	float x, y;
	entity->getPosition(&x, &y);

	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);

	float ix, iy, t;

	// are we moving?
	float magSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magSqr > 0) {

		// loop through all obstacles and find collisions
		for (auto& obstacle : m_obstacles) {

			if (obstacle.type == Obstacle::SPHERE) {
				if (intersection::rayCircleIntersection(x, y,
										  velocity->x, velocity->y,
										  obstacle.x, obstacle.y, obstacle.r,
										  ix, iy, &t)) {
					// within range?
					if (t >= 0 &&
						t <= m_feelerLength) {
						force.x += (ix - obstacle.x) / obstacle.r;
						force.y += (iy - obstacle.y) / obstacle.r;
					}
				}

				// rotate feeler about 30 degrees
				float s = sinf(3.14159f*0.15f);
				float c = cosf(3.14159f*0.15f);
				if (intersection::rayCircleIntersection(x, y,
										  velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c, // apply rotation to vector
										  obstacle.x, obstacle.y, obstacle.r,
										  ix, iy, &t)) {
					if (t >= 0 &&
						t <= (m_feelerLength * 0.5f)) { // scale feeler 50%
						force.x += (ix - obstacle.x) / obstacle.r;
						force.y += (iy - obstacle.y) / obstacle.r;
					}
				}

				// rotate feeler about -30 degrees
				s = sinf(3.14159f*-0.15f);
				c = cosf(3.14159f*-0.15f);
				if (intersection::rayCircleIntersection(x, y,
										  velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c, // apply rotation to vector
										  obstacle.x, obstacle.y, obstacle.r,
										  ix, iy, &t)) {
					if (t >= 0 &&
						t <= (m_feelerLength * 0.5f)) { // scale feeler 50%
						force.x += (ix - obstacle.x) / obstacle.r;
						force.y += (iy - obstacle.y) / obstacle.r;
					}
				}
			}
			else if (obstacle.type == Obstacle::BOX) {
				float nx = 0, ny = 0;

				float mag = sqrt(magSqr);

				if (intersection::rayBoxIntersection(x, y,
									   velocity->x / mag * m_feelerLength, velocity->y / mag * m_feelerLength,
									   obstacle.x - obstacle.w * 0.5f, obstacle.y - obstacle.h * 0.5f, obstacle.w, obstacle.h,
									   nx, ny,
									   &t)) {
					force.x += nx;
					force.y += ny;
				}

				// rotate feeler about 30 degrees
				float s = sinf(3.14159f*0.15f);
				float c = cosf(3.14159f*0.15f);
				if (intersection::rayBoxIntersection(x, y,
									   (velocity->x * c - velocity->y * s) / mag * m_feelerLength * 0.5f,
									   (velocity->x * s + velocity->y * c) / mag * m_feelerLength * 0.5f,
									   obstacle.x - obstacle.w * 0.5f, obstacle.y - obstacle.h * 0.5f, obstacle.w, obstacle.h,
									   nx, ny,
									   &t)) {
					force.x += nx;
					force.y += ny;
				}
				// rotate feeler about 30 degrees
				s = sinf(3.14159f*-0.15f);
				c = cosf(3.14159f*-0.15f);
				if (intersection::rayBoxIntersection(x, y,
									   (velocity->x * c - velocity->y * s) / mag * m_feelerLength * 0.5f,
									   (velocity->x * s + velocity->y * c) / mag * m_feelerLength * 0.5f,
									   obstacle.x - obstacle.w * 0.5f, obstacle.y - obstacle.h * 0.5f, obstacle.w, obstacle.h,
									   nx, ny,
									   &t)) {
					force.x += nx;
					force.y += ny;
				}
			}
		}
	}
	
	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return{ force.x * maxForce, force.y * maxForce };
}*/

glm::vec3 SeparationForce::getForce(Entity* entity) const {
	
	// get my position
	auto position = entity->getPosition();

	glm::vec3 force(0);
	int neighbours = 0;

	for (auto& e : *m_entities) {

		if (&e == entity) continue;

		auto target = e.getPosition();

		// compare the two and get the distance between them
		auto diff = position - target;
		float distanceSqr = glm::dot(diff, diff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {

			// push away from entity!
			diff = glm::normalize(diff);

			neighbours++;
			force += diff;
		}
	}

	if (neighbours > 0)
		force /= (float)neighbours;

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return force * maxForce;
}

glm::vec3 CohesionForce::getForce(Entity* entity) const {

	// get my position
	auto position = entity->getPosition();

	glm::vec3 force(0);
	int neighbours = 0;

	for (auto& e : *m_entities) {

		if (&e == entity) continue;

		auto target = e.getPosition();

		// compare the two and get the distance between them
		auto diff = position - target;
		float distanceSqr = glm::dot(diff, diff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {
			neighbours++;
			force += target;
		}
	}

	if (neighbours > 0) {

		force = force / (float)neighbours - position;

		// normalise direction
		if (glm::dot(force, force) > 0)
			force = glm::normalize(force);
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return force * maxForce;
}

glm::vec3 AlignmentForce::getForce(Entity* entity) const {

	// get my position
	auto position = entity->getPosition();

	glm::vec3 force(0);
	int neighbours = 0;

	for (auto& e : *m_entities) {

		if (&e == entity) continue;

		auto target = e.getPosition();

		// compare the two and get the distance between them
		auto diff = position - target;
		float distanceSqr = glm::dot(diff, diff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {

			glm::vec3* v = nullptr;
			e.getBlackboard().get("velocity", &v);

			distanceSqr = glm::dot(*v, *v);
			if (distanceSqr > 0) {
				neighbours++;
				force += *v;
			}
		}
	}

	if (neighbours > 0) {

		glm::vec3* v = nullptr;
		entity->getBlackboard().get("velocity", &v);

		force = force / (float)neighbours - *v;

		// normalise direction
		if (glm::dot(force, force) > 0)
			force = glm::normalize(force);
	}

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return force * maxForce;
}

glm::vec3 FlowForce::getForce(Entity* entity) const {

	if (m_flowField == nullptr)
		return {};

	auto position = entity->getPosition();

	glm::ivec3 cell = position / m_cellSize;

	// off-grid?
	if (cell < glm::ivec3(0) ||
		cell >= glm::ivec3(m_cellSize, m_cellSize, m_cellSize))
		return {};

	int index = cell.z * (m_cols * m_rows) + cell.y * m_cols + cell.x;

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return m_flowField[index] * maxForce;
}

} // namespace ai