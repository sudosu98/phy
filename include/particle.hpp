#pragma once

#include "vector3d.hpp"
#include <string>

/**
 * Represents a particle in the physics simulation
 */
class Particle {
public:
    /**
     * Constructor
     * @param mass The mass of the particle
     * @param position Initial position
     * @param velocity Initial velocity
     * @param name Optional name for the particle
     */
    Particle(double mass, const Vector3D& position, const Vector3D& velocity, const std::string& name = "");
    
    /**
     * Destructor
     */
    virtual ~Particle() = default;
    
    /**
     * Apply a force to the particle
     * @param force The force vector to apply
     */
    void applyForce(const Vector3D& force);
    
    /**
     * Update the particle's position based on its velocity
     * @param dt Time step in seconds
     */
    void updatePosition(double dt);
    
    /**
     * Update the particle's velocity based on accumulated forces
     * @param dt Time step in seconds
     */
    void updateVelocity(double dt);
    
    /**
     * Reset accumulated forces to zero
     */
    void resetForces();
    
    // Getters
    double getMass() const { return mass_; }
    const Vector3D& getPosition() const { return position_; }
    const Vector3D& getVelocity() const { return velocity_; }
    const Vector3D& getForce() const { return force_; }
    const std::string& getName() const { return name_; }
    
    // Setters
    void setPosition(const Vector3D& position) { position_ = position; }
    void setVelocity(const Vector3D& velocity) { velocity_ = velocity; }
    
private:
    double mass_;           // Mass of the particle
    Vector3D position_;     // Current position
    Vector3D velocity_;     // Current velocity
    Vector3D force_;        // Accumulated force
    std::string name_;      // Optional name for identification
}; 