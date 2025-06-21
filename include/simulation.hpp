#pragma once

#include <vector>
#include <memory>
#include "vector3d.hpp"
#include "particle.hpp"

/**
 * Main simulation class that handles the physics simulation
 */
class Simulation {
public:
    Simulation();
    ~Simulation();
    
    /**
     * Initialize the simulation with default parameters
     */
    void initialize();
    
    /**
     * Advance the simulation by the specified time step
     * @param dt Time step in seconds
     */
    void step(double dt);
    
    /**
     * Print the current state of the simulation
     */
    void printState() const;
    
    /**
     * Get access to the particles in the simulation
     * @return Reference to the vector of particles
     */
    const std::vector<std::unique_ptr<Particle>>& getParticles() const {
        return particles_;
    }
    
    /**
     * Get the central particle for direct manipulation
     * @return Pointer to the central particle, or nullptr if no particles exist
     */
    Particle* getCentralParticle();
    
private:
    /**
     * Apply forces between particles
     */
    void applyForces();
    
    /**
     * Update particle positions based on velocities
     * @param dt Time step in seconds
     */
    void updatePositions(double dt);
    
    /**
     * Update particle velocities based on forces
     * @param dt Time step in seconds
     */
    void updateVelocities(double dt);
    
    // Simulation parameters
    double gravity_;
    double damping_;
    
    // Collection of particles in the simulation
    std::vector<std::unique_ptr<Particle>> particles_;
}; 