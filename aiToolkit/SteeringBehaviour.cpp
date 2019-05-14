#include "SteeringBehaviour.h"
#include "Intersection.h"
#include "Blackboard.h"
#include "Timing.h"
#include "Geometry.h"
#include <glm/ext.hpp>

namespace ai {

eBehaviourResult SteeringBehaviour::execute(Agent* entity) {

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
	if (glm::dot(*velocity, *velocity) > (maxVelocity * maxVelocity))
		*velocity = glm::normalize(*velocity) * maxVelocity;

	entity->translate(*velocity * app::Time::deltaTime());

	return eBehaviourResult::SUCCESS;
}

void SteeringState::update(Agent* entity) {

	glm::vec3 force(0);

	// must have velocity
	glm::vec3* velocity = nullptr;
	if (entity->getBlackboard().get("velocity", &velocity) == false)
		return;

	// accumulate forces
	for (auto& wf : m_forces)
		force += wf.force->getForce(entity) * wf.weight;

	float maxVelocity = 0;
	entity->getBlackboard().get("maxVelocity", maxVelocity);

	*velocity += force * app::Time::deltaTime();

	// cap velocity
	if (glm::dot(*velocity, *velocity) > (maxVelocity * maxVelocity))
		*velocity = glm::normalize(*velocity) * maxVelocity;

	entity->translate(*velocity * app::Time::deltaTime());
}

glm::vec3 SeekForce::getForce(Agent* entity) const {

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

glm::vec3 FleeForce::getForce(Agent* entity) const {

	// get target position
	auto target = m_target->getPosition();

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

glm::vec3 PursueForce::getForce(Agent* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get target's velocity
	glm::vec3* velocity = nullptr;
	m_target->getBlackboard().get("velocity", &velocity);

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

glm::vec3 EvadeForce::getForce(Agent* entity) const {

	// get target position
	auto target = m_target->getPosition();

	// get target's velocity
	glm::vec3* velocity = nullptr;
	m_target->getBlackboard().get("velocity", &velocity);

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

glm::vec3 WanderForce::getForce(Agent* entity) const {

	WanderData* wd = nullptr;
	if (entity->getBlackboard().get("wanderData", &wd) == false) {
		return glm::vec3(0);
	}

	auto wander = wd->target;

	// apply the jitter to our current wander target
	// generate a random circular direction with a radius of "jitter"
	wander += glm::normalize(glm::sphericalRand(1.0f) * wd->axisWeights) * wd->jitter;

	// bring it back to a radius around the game object
	wander = glm::normalize(wander) * wd->radius;

	// store the target back into the game object
	wd->target = wander;

	// access the game object's velocity as a unit vector (normalised)
	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);

	// combine velocity direction with wander target to offset
	if (glm::dot(*velocity, *velocity) > 0)	
		wander += glm::normalize(*velocity) * wd->offset;

	// normalise the new direction
	if (glm::dot(wander, wander) > 0)
		wander = glm::normalize(wander);

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return wander * maxForce;
}

glm::vec3 ObstacleAvoidanceForce::getForce(ai::Agent* entity) const {

	glm::vec3 force(0);

	// create feeler
	auto position = entity->getPosition();

	glm::vec3* velocity = nullptr;
	entity->getBlackboard().get("velocity", &velocity);

	glm::vec3 i;
	float t;

	// are we moving?
	float magSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magSqr > 0) {

		// loop through all obstacles and find collisions
		for (auto& obstacle : m_obstacles) {

			if (obstacle.type == Obstacle::SPHERE) {
				if (intersection::rayCircleIntersection(position,
										  *velocity,
										  obstacle.center, obstacle.radius,
										  i, &t)) {
					// within range?
					if (t >= 0 &&
						t <= m_feelerLength) 
						force += (i - obstacle.center) / obstacle.radius;
				}

				// rotate feeler about 30 degrees
				float s = sinf(3.14159f*0.15f);
				float c = cosf(3.14159f*0.15f);
				if (intersection::rayCircleIntersection(position,
					{ velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c, 0 }, // apply rotation to vector
										  obstacle.center, obstacle.radius,
										  i, &t)) {
					if (t >= 0 &&
						t <= (m_feelerLength * 0.5f))  // scale feeler 50%
						force += (i - obstacle.center) / obstacle.radius;
				}

				// rotate feeler about -30 degrees
				s = sinf(3.14159f*-0.15f);
				c = cosf(3.14159f*-0.15f);
				if (intersection::rayCircleIntersection(position,
					{ velocity->x * c - velocity->y * s, velocity->x * s + velocity->y * c,0 }, // apply rotation to vector
										  obstacle.center, obstacle.radius,
										  i, &t)) {
					if (t >= 0 &&
						t <= (m_feelerLength * 0.5f)) // scale feeler 50%
						force += (i - obstacle.center) / obstacle.radius;
				}
			}
			else if (obstacle.type == Obstacle::BOX) {
				glm::vec3 n(0);

				float mag = sqrt(magSqr);

				if (intersection::rayBoxIntersection(position,
					{velocity->x / mag * m_feelerLength, velocity->y / mag * m_feelerLength,0},
									   obstacle.center - obstacle.extents * 0.5f, obstacle.extents,
									   n,
									   &t)) {
					force += n;
				}

				// rotate feeler about 30 degrees
				float s = sinf(3.14159f*0.15f);
				float c = cosf(3.14159f*0.15f);
				if (intersection::rayBoxIntersection(position,
					{ (velocity->x * c - velocity->y * s) / mag * m_feelerLength * 0.5f,
									   (velocity->x * s + velocity->y * c) / mag * m_feelerLength * 0.5f,0 },
					obstacle.center - obstacle.extents * 0.5f, obstacle.extents,
									   n,
									   &t)) {
					force += n;
				}
				// rotate feeler about 30 degrees
				s = sinf(3.14159f*-0.15f);
				c = cosf(3.14159f*-0.15f);
				if (intersection::rayBoxIntersection(position,
					{ (velocity->x * c - velocity->y * s) / mag * m_feelerLength * 0.5f,
									   (velocity->x * s + velocity->y * c) / mag * m_feelerLength * 0.5f,0 },
					obstacle.center - obstacle.extents * 0.5f, obstacle.extents,
									   n,
									   &t)) {
					force += n;
				}
			}
		}
	}
	
	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return force * maxForce;
}

glm::vec3 SeparationForce::getForce(Agent* entity) const {
	
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

glm::vec3 CohesionForce::getForce(Agent* entity) const {

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

glm::vec3 AlignmentForce::getForce(Agent* entity) const {

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

			if (glm::dot(*v, *v) > 0) {
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

glm::vec3 FlowForce::getForce(Agent* entity) const {

	if (m_flowField == nullptr)
		return {};

	auto position = entity->getPosition();

	glm::ivec3 cell = position / m_cellSize;

	// off-grid?
	if (cell < glm::ivec3(0) ||
		cell >= glm::ivec3(m_cols, m_rows, m_depth))
		return {};

	int index = cell.z * (m_cols * m_rows) + cell.y * m_cols + cell.x;

	float maxForce = 0;
	entity->getBlackboard().get("maxForce", maxForce);

	return m_flowField[index] * maxForce;
}

} // namespace ai