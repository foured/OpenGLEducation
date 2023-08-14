#include "rigidbody.h"

RigidBody::RigidBody() {}

RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos)
	: modelId(modelId), size(size), mass(mass), pos(pos), acceleration(0.0f), velocity(0.0f), state(0) {}

void RigidBody::update(float dt) {
	pos += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
}

void RigidBody::applyForce(glm::vec3 force) {
	acceleration += force / mass;
}

void RigidBody::applyForce(glm::vec3 direction, float magnitude) {
	applyForce(direction * magnitude);
}

void RigidBody::applyImpuls(glm::vec3 force, float dt) {
	velocity += force / mass * dt;
}

void RigidBody::applyImpuls(glm::vec3 direction, float magnitude, float dt) {
	applyImpuls(direction * magnitude, dt);
}

void RigidBody::transferEnergy(float joules, glm::vec3 direction) {
	if (joules == 0) return;

	glm::vec3 deltaV = sqrt(2 * abs(joules) / mass) * direction;

	velocity += joules > 0 ? deltaV : -deltaV;
}

void RigidBody::applyAcceleration(glm::vec3 a) {
	acceleration += a;
}


void RigidBody::applyAcceleration(glm::vec3 direction, float magnitude) {
	applyAcceleration(direction * magnitude);
}