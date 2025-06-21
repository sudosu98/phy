#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h> // macOS path
#else
#include <GL/freeglut.h> // Linux/Windows path
#endif
#include "simulation.hpp"


/**
 * Simple struct to represent a rectangular obstacle
 */
struct Obstacle {
    float x, y;        // Center position
    float width, height; // Dimensions
    
    Obstacle(float x, float y, float width, float height)
        : x(x), y(y), width(width), height(height) {}
        
    // Check if a point is inside the obstacle
    bool contains(float px, float py) const {
        return (px >= x - width/2 && px <= x + width/2 &&
                py >= y - height/2 && py <= y + height/2);
    }
};

/**
 * Class to visualize the physics simulation using OpenGL
 */
class GLVisualizer {
public:
    /**
     * Constructor
     * @param simulation Reference to the simulation to visualize
     * @param width Window width
     * @param height Window height
     * @param title Window title
     */
    GLVisualizer(Simulation& simulation, unsigned int width = 800, unsigned int height = 600, 
               const std::string& title = "Physics Simulation");
    
    /**
     * Destructor
     */
    ~GLVisualizer();
    
    /**
     * Run the visualization loop
     */
    void run();
    
    // Key state variables (public for callback access)
    bool keyW_, keyA_, keyS_, keyD_;
    bool keyK_, keyL_; // K and L keys for rotation
    
private:
    /**
     * Initialize OpenGL
     */
    bool initGL();
    
    /**
     * Process a single frame of the visualization
     */
    void update();
    
    /**
     * Render the current state of the simulation
     */
    void render();
    
    /**
     * Draw a circle at the given position with the given radius
     * @param x X coordinate
     * @param y Y coordinate
     * @param radius Radius of the circle
     * @param segments Number of segments to use for the circle
     */
    void drawCircle(float x, float y, float radius, int segments);
    
    /**
     * Draw a torch light effect in the direction of the mouse
     * @param x X coordinate of the base
     * @param y Y coordinate of the base
     * @param dirX X component of the direction
     * @param dirY Y component of the direction
     * @param length Base length of the torch light
     */
    void drawTorch(float x, float y, float dirX, float dirY, float length);
    
    /**
     * Draw text at the specified position
     * @param text The text to draw
     * @param x X coordinate
     * @param y Y coordinate
     */
    void drawText(const std::string& text, float x, float y);
    
    /**
     * Draw location labels on the map
     */
    void drawLocationLabels();
    
    /**
     * Draw site markers (A and B) on the map
     */
    void drawSiteMarkers();
    
    /**
     * Draw a grid overlay on the map for tactical look
     */
    void drawGrid();
    
    /**
     * Draw a rectangle (used for obstacles)
     * @param x Center X coordinate
     * @param y Center Y coordinate
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     */
    void drawRectangle(float x, float y, float width, float height);
    
    /**
     * Initialize obstacles in the map
     */
    void initializeObstacles();
    
    /**
     * Place the particle in a valid position that doesn't collide with obstacles
     */
    void placeParticleInValidPosition();
    
    /**
     * Check if a position collides with any obstacle
     * @param x X coordinate to check
     * @param y Y coordinate to check
     * @param radius Radius of the object to check
     * @return true if collision detected, false otherwise
     */
    bool checkObstacleCollision(float x, float y, float radius);
    
    /**
     * Handle keyboard input
     */
    void handleKeyboardInput();
    
    /**
     * Update the direction based on keyboard input (K and L keys)
     */
    void updateDirection();
    
    // GLFW window
    GLFWwindow* window_;
    
    // Reference to the simulation
    Simulation& simulation_;
    
    // Window dimensions
    unsigned int width_;
    unsigned int height_;
    
    // Visualization parameters
    const float particleRadius_ = 0.3f; // Reduced particle size (was 0.5f)
    int particleSegments_;
    
    // Direction vector (normalized)
    float directionX_;
    float directionY_;
    float rotationAngle_; // Current rotation angle in radians
    float rotationSpeed_; // Rotation speed in radians per frame
    
    // Movement speed
    float moveSpeed_;
    
    // Mouse position
    double mouseX_, mouseY_;
    
    // Obstacles in the map
    std::vector<Obstacle> obstacles_;
}; 