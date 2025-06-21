#include "particle.hpp"

Particle::Particle(double mass, const Vector3D& position, const Vector3D& velocity, const std::string& name)
    : mass_(mass), position_(position), velocity_(velocity), force_(), name_(name) {
    if (mass <= 0) {
        throw std::invalid_argument("Particle mass must be positive");
    }
}

void Particle::applyForce(const Vector3D& force) {
    force_ += force;
}

void Particle::updatePosition(double dt) {
    // Simple Euler integration: position += velocity * dt
    position_ += velocity_ * dt;
}

void Particle::updateVelocity(double dt) {
    // F = ma, so a = F/m
    // v += a * dt
    Vector3D acceleration = force_ / mass_;
    velocity_ += acceleration * dt;
}

void Particle::resetForces() {
    force_ = Vector3D(0, 0, 0);
} 