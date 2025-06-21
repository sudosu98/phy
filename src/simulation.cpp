#include "simulation.hpp"
#include <iostream>
#include <stdexcept>
#include <random>
#include <cmath> // Added for M_PI and std::sqrt

Simulation::Simulation() 
    : gravity_(0.0), damping_(0.0) {  // Set damping to 0 to prevent any slowdown effect
}

Simulation::~Simulation() {
    // Clean up resources if needed
}

void Simulation::initialize() {
    // Clear existing particles
    particles_.clear();
    
    // Create a single central particle with no initial velocity
    Vector3D position(0, 0, 0);
    Vector3D velocity(0.0, 0.0, 0); // No initial velocity
    double mass = 10.0;
    
    particles_.push_back(std::make_unique<Particle>(mass, position, velocity, "CentralParticle"));
    
    std::cout << "Initialized simulation with " << particles_.size() << " particle." << std::endl;
}

void Simulation::step(double dt) {
    if (dt <= 0) {
        throw std::invalid_argument("Time step must be positive");
    }
    
    // Apply forces
    applyForces();
    
    // Update velocities based on forces
    updateVelocities(dt);
    
    // Update positions based on velocities
    updatePositions(dt);
}

void Simulation::applyForces() {
    // Reset all forces
    for (auto& particle : particles_) {
        particle->resetForces();
    }
    
    // No forces applied - particle movement is controlled directly through velocity
}

void Simulation::updateVelocities(double dt) {
    for (auto& particle : particles_) {
        particle->updateVelocity(dt);
    }
}

void Simulation::updatePositions(double dt) {
    for (auto& particle : particles_) {
        particle->updatePosition(dt);
    }
}

void Simulation::printState() const {
    std::cout << "=== Simulation State ===" << std::endl;
    for (const auto& particle : particles_) {
        std::cout << particle->getName() << ": "
                  << "Position: " << particle->getPosition() 
                  << ", Velocity: " << particle->getVelocity()
                  << ", Mass: " << particle->getMass() << std::endl;
    }
    std::cout << "======================" << std::endl;
} 

// Method to get the central particle
Particle* Simulation::getCentralParticle() {
    if (!particles_.empty()) {
        return particles_[0].get();
    }
    return nullptr;
} 