#include <gtest/gtest.h>
#include "vector3d.hpp"
#include "particle.hpp"
#include "simulation.hpp"

// Test Vector3D class
TEST(Vector3DTest, Construction) {
    Vector3D v1;
    EXPECT_DOUBLE_EQ(v1.x, 0.0);
    EXPECT_DOUBLE_EQ(v1.y, 0.0);
    EXPECT_DOUBLE_EQ(v1.z, 0.0);
    
    Vector3D v2(1.0, 2.0, 3.0);
    EXPECT_DOUBLE_EQ(v2.x, 1.0);
    EXPECT_DOUBLE_EQ(v2.y, 2.0);
    EXPECT_DOUBLE_EQ(v2.z, 3.0);
}

TEST(Vector3DTest, Addition) {
    Vector3D v1(1.0, 2.0, 3.0);
    Vector3D v2(4.0, 5.0, 6.0);
    
    Vector3D result = v1 + v2;
    EXPECT_DOUBLE_EQ(result.x, 5.0);
    EXPECT_DOUBLE_EQ(result.y, 7.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST(Vector3DTest, Subtraction) {
    Vector3D v1(5.0, 7.0, 9.0);
    Vector3D v2(1.0, 2.0, 3.0);
    
    Vector3D result = v1 - v2;
    EXPECT_DOUBLE_EQ(result.x, 4.0);
    EXPECT_DOUBLE_EQ(result.y, 5.0);
    EXPECT_DOUBLE_EQ(result.z, 6.0);
}

TEST(Vector3DTest, ScalarMultiplication) {
    Vector3D v(1.0, 2.0, 3.0);
    
    Vector3D result = v * 2.0;
    EXPECT_DOUBLE_EQ(result.x, 2.0);
    EXPECT_DOUBLE_EQ(result.y, 4.0);
    EXPECT_DOUBLE_EQ(result.z, 6.0);
    
    // Test the other direction
    result = 3.0 * v;
    EXPECT_DOUBLE_EQ(result.x, 3.0);
    EXPECT_DOUBLE_EQ(result.y, 6.0);
    EXPECT_DOUBLE_EQ(result.z, 9.0);
}

TEST(Vector3DTest, Magnitude) {
    Vector3D v(3.0, 4.0, 0.0);
    EXPECT_DOUBLE_EQ(v.magnitude(), 5.0);
    
    Vector3D v2(1.0, 1.0, 1.0);
    EXPECT_DOUBLE_EQ(v2.magnitude(), std::sqrt(3.0));
}

// Test Particle class
TEST(ParticleTest, Construction) {
    Vector3D pos(1.0, 2.0, 3.0);
    Vector3D vel(4.0, 5.0, 6.0);
    
    Particle p(10.0, pos, vel, "TestParticle");
    
    EXPECT_DOUBLE_EQ(p.getMass(), 10.0);
    EXPECT_EQ(p.getPosition().x, pos.x);
    EXPECT_EQ(p.getPosition().y, pos.y);
    EXPECT_EQ(p.getPosition().z, pos.z);
    EXPECT_EQ(p.getVelocity().x, vel.x);
    EXPECT_EQ(p.getVelocity().y, vel.y);
    EXPECT_EQ(p.getVelocity().z, vel.z);
    EXPECT_EQ(p.getName(), "TestParticle");
}

TEST(ParticleTest, ForceApplication) {
    Vector3D pos(0.0, 0.0, 0.0);
    Vector3D vel(0.0, 0.0, 0.0);
    
    Particle p(2.0, pos, vel);
    
    // Apply a force
    Vector3D force(10.0, 0.0, 0.0);
    p.applyForce(force);
    
    // Check that force is stored
    EXPECT_DOUBLE_EQ(p.getForce().x, 10.0);
    EXPECT_DOUBLE_EQ(p.getForce().y, 0.0);
    EXPECT_DOUBLE_EQ(p.getForce().z, 0.0);
    
    // Update velocity (F=ma -> a=F/m -> v+=a*dt)
    double dt = 0.1;
    p.updateVelocity(dt);
    
    // Expected acceleration = 10/2 = 5, so velocity change = 5*0.1 = 0.5
    EXPECT_DOUBLE_EQ(p.getVelocity().x, 0.5);
    EXPECT_DOUBLE_EQ(p.getVelocity().y, 0.0);
    EXPECT_DOUBLE_EQ(p.getVelocity().z, 0.0);
    
    // Update position
    p.updatePosition(dt);
    
    // Expected position change = 0.5*0.1 = 0.05
    EXPECT_DOUBLE_EQ(p.getPosition().x, 0.05);
    EXPECT_DOUBLE_EQ(p.getPosition().y, 0.0);
    EXPECT_DOUBLE_EQ(p.getPosition().z, 0.0);
}

// Main function to run all tests
int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 