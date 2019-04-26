#include "SteeringBehaviour.h"

#include "aiUtilities.h"
#include "Blackboard.h"

#include <glm/ext.hpp>

eBehaviourResult SteeringBehaviour::execute(GameObject* gameObject, float deltaTime) {

	Force force = { 0,0 };

	Vector2* velocity = nullptr;
	// must have velocity
	if (gameObject->getBlackboard().get("velocity", &velocity) == false)
		return eBehaviourResult::FAILURE;

	for (auto& wf : m_forces) {
		Force temp = wf.force->getForce(gameObject);

		// accumulate forces
		force.x += temp.x * wf.weight;
		force.y += temp.y * wf.weight;
	}

	float maxVelocity = 0;
	gameObject->getBlackboard().get("maxVelocity", maxVelocity);

	velocity->x += force.x * deltaTime;
	velocity->y += force.y * deltaTime;

	// cap velocity
	float magnitudeSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magnitudeSqr > (maxVelocity * maxVelocity)) {
		float magnitude = sqrt(magnitudeSqr);
		velocity->x = velocity->x / magnitude * maxVelocity;
		velocity->y = velocity->y / magnitude * maxVelocity;
	}

	gameObject->translate(velocity->x * deltaTime, velocity->y * deltaTime);

	return eBehaviourResult::SUCCESS;
}

void SteeringState::update(GameObject* gameObject, float deltaTime) {

	Force force = { 0,0 };

	for (auto& wf : m_forces) {
		Force temp = wf.force->getForce(gameObject);

		// accumulate forces
		force.x += temp.x * wf.weight;
		force.y += temp.y * wf.weight;
	}

	float maxVelocity = 0;
	gameObject->getBlackboard().get("maxVelocity", maxVelocity);

	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	velocity->x += force.x * deltaTime;
	velocity->y += force.y * deltaTime;

	// cap velocity
	float magnitudeSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magnitudeSqr > (maxVelocity * maxVelocity)) {
		float magnitude = sqrt(magnitudeSqr);
		velocity->x = velocity->x / magnitude * maxVelocity;
		velocity->y = velocity->y / magnitude * maxVelocity;
	}

	gameObject->translate(velocity->x * deltaTime, velocity->y * deltaTime);
}

Force SeekForce::getForce(GameObject* gameObject) const {

	// get target position
	float targetX = 0, targetY = 0;
	m_target->getPosition(&targetX, &targetY);

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// get a vector to the target from "us"
	float xDiff = targetX - x;
	float yDiff = targetY - y;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return { xDiff * maxForce, yDiff * maxForce };
}

Force FleeForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = x - tx;
	float yDiff = y - ty;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

Force PursueForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get target's velocity
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	// add velocity to target
	tx += velocity->x;
	ty += velocity->y;

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = tx - x;
	float yDiff = ty - y;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

Force EvadeForce::getForce(GameObject* gameObject) const {

	// get target position
	float tx = 0, ty = 0;
	m_target->getPosition(&tx, &ty);

	// get target's velocity
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	// add velocity to target
	tx += velocity->x;
	ty += velocity->y;

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	// compare the two and get the distance between them
	float xDiff = x - tx;
	float yDiff = y - ty;
	float distance = (xDiff * xDiff + yDiff * yDiff);

	// if not at the target then move towards them
	if (distance > 0) {

		distance = sqrt(distance);

		// need to make the difference the length of 1
		// this is so movement can be "pixels per second"
		xDiff /= distance;
		yDiff /= distance;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ xDiff * maxForce, yDiff * maxForce };
}

Force WanderForce::getForce(GameObject* gameObject) const {

	WanderData* wd = nullptr;
	if (gameObject->getBlackboard().get("wanderData", &wd) == false) {
		return{ 0,0 };
	}

	// generate a random circular direction with a radius of "jitter"
	glm::vec2 jitterOffset = glm::circularRand(wd->jitter);

	float wanderX = wd->x, wanderY = wd->y;

	// apply the jitter to our current wander target
	wanderX += jitterOffset.x;
	wanderY += jitterOffset.y;

	// bring it back to a radius around the game object
	float magnitude = sqrt(wanderX * wanderX + wanderY * wanderY);
	wanderX = (wanderX / magnitude) * wd->radius;
	wanderY = (wanderY / magnitude) * wd->radius;

	// store the target back into the game object
	wd->x = wanderX;
	wd->y = wanderY;

	// access the game object's velocity as a unit vector (normalised)
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);
	float vx = velocity->x;
	float vy = velocity->y;

	// normalise and protect from divide-by-zero
	magnitude = vx * vx + vy * vy;
	if (magnitude > 0) {
		magnitude = sqrt(magnitude);
		vx /= magnitude;
		vy /= magnitude;
	}
	
	// combine velocity direction with wander target to offset
	wanderX += vx * wd->offset;
	wanderY += vy * wd->offset;

	// normalise the new direction
	magnitude = wanderX * wanderX + wanderY * wanderY;
	if (magnitude > 0) {

		magnitude = sqrt(magnitude);
		wanderX /= magnitude;
		wanderY /= magnitude;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ wanderX * maxForce, wanderY * maxForce };
}

Force ObstacleAvoidanceForce::getForce(GameObject* gameObject) const {

	Force force = {0, 0};

	// create feeler
	float x, y;
	gameObject->getPosition(&x, &y);
	Vector2* velocity = nullptr;
	gameObject->getBlackboard().get("velocity", &velocity);

	float ix, iy, t;

	// are we moving?
	float magSqr = velocity->x * velocity->x + velocity->y * velocity->y;
	if (magSqr > 0) {

		// loop through all obstacles and find collisions
		for (auto& obstacle : m_obstacles) {

			if (obstacle.type == Obstacle::SPHERE) {
				if (rayCircleIntersection(x, y,
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
				if (rayCircleIntersection(x, y,
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
				if (rayCircleIntersection(x, y,
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

				if (rayBoxIntersection(x, y,
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
				if (rayBoxIntersection(x, y,
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
				if (rayBoxIntersection(x, y,
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
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ force.x * maxForce, force.y * maxForce };
}

Force SeparationForce::getForce(GameObject* gameObject) const {
	
	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	Force force = {};
	int neighbours = 0;

	for (auto& entity : *m_entities) {

		if (gameObject == &entity) continue;

		float tx = 0, ty = 0;
		entity.getPosition(&tx, &ty);

		// compare the two and get the distance between them
		float xDiff = x - tx;
		float yDiff = y - ty;
		float distanceSqr = (xDiff * xDiff + yDiff * yDiff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {

			// push away from entity!
			distanceSqr = sqrt(distanceSqr);

			// need to make the difference the length of 1
			// this is so movement can be "pixels per second"
			xDiff /= distanceSqr;
			yDiff /= distanceSqr;

			neighbours++;
			force.x += xDiff;
			force.y += yDiff;
		}
	}

	if (neighbours > 0) {

		force.x /= neighbours;
		force.y /= neighbours;
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return { force.x * maxForce, force.y * maxForce };
}

Force CohesionForce::getForce(GameObject* gameObject) const {

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	Force force = {};
	int neighbours = 0;

	for (auto& entity : *m_entities) {

		if (gameObject == &entity) continue;

		float tx = 0, ty = 0;
		entity.getPosition(&tx, &ty);

		// compare the two and get the distance between them
		float xDiff = x - tx;
		float yDiff = y - ty;
		float distanceSqr = (xDiff * xDiff + yDiff * yDiff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {

			neighbours++;
			force.x += tx;
			force.y += ty;
		}
	}

	if (neighbours > 0) {

		force.x = force.x / neighbours - x;
		force.y = force.y / neighbours - y;

		// normalise direction
		float d = force.x * force.x + force.y * force.y;
		if (d > 0) {
			d = sqrt(d);
			force.x /= d;
			force.y /= d;
		}
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ force.x * maxForce, force.y * maxForce };
}

Force AlignmentForce::getForce(GameObject* gameObject) const {

	// get my position
	float x = 0, y = 0;
	gameObject->getPosition(&x, &y);

	Force force = {};
	int neighbours = 0;

	for (auto& entity : *m_entities) {

		if (gameObject == &entity) continue;

		float tx = 0, ty = 0;
		entity.getPosition(&tx, &ty);

		// compare the two and get the distance between them
		float xDiff = x - tx;
		float yDiff = y - ty;
		float distanceSqr = (xDiff * xDiff + yDiff * yDiff);

		// is it within radius?
		if (distanceSqr > 0 &&
			distanceSqr < (m_radius * m_radius)) {

			Vector2* v = nullptr;
			entity.getBlackboard().get("velocity", &v);

			distanceSqr = v->x * v->x + v->y * v->y;
			if (distanceSqr > 0) {
				distanceSqr = sqrt(distanceSqr);


				
				neighbours++;
				force.x += v->x;
				force.y += v->y;
			}
		}
	}

	if (neighbours > 0) {

		Vector2* v = nullptr;
		gameObject->getBlackboard().get("velocity", &v);

		force.x = force.x / neighbours - v->x;
		force.y = force.y / neighbours - v->y;

		// normalise direction
		float d = force.x * force.x + force.y * force.y;
		if (d > 0) {
			d = sqrt(d);
			force.x /= d;
			force.y /= d;
		}
	}

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ force.x * maxForce, force.y * maxForce };
}

Force FlowForce::getForce(GameObject* gameObject) const {

	if (m_flowField == nullptr)
		return { 0, 0 };

	float x, y;
	gameObject->getPosition(&x, &y);

	int ix = int(x / m_cellSize);
	int iy = int(y / m_cellSize);

	// off-grid?
	if (ix < 0 ||
		ix >= m_cols ||
		iy < 0 ||
		iy >= m_rows)
		return { 0,0 };

	int index = iy * m_cols + ix;

	float maxForce = 0;
	gameObject->getBlackboard().get("maxForce", maxForce);

	return{ m_flowField[index].x * maxForce, m_flowField[index].y * maxForce };
}