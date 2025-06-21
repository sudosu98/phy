#define GL_SILENCE_DEPRECATION // Silence OpenGL deprecation warnings on macOS

#include "gl_visualizer.hpp"
#include <iostream>
#include <cmath>
#include <cstdlib> // Added for rand()
#include <ctime> // Added for time()
#include <vector> // Added for std::vector
#include <random> // Added for random number generation


// Error callback for GLFW
static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

// Key callback for GLFW
static void keyCallback(GLFWwindow* window, int key, int scancode [[maybe_unused]], int action, int mods [[maybe_unused]]) {
    GLVisualizer* visualizer = static_cast<GLVisualizer*>(glfwGetWindowUserPointer(window));
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    
    // Track key states for WASD movement
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) visualizer->keyW_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyW_ = false;
    }
    else if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) visualizer->keyA_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyA_ = false;
    }
    else if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) visualizer->keyS_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyS_ = false;
    }
    else if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) visualizer->keyD_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyD_ = false;
    }
    // Track key states for rotation
    else if (key == GLFW_KEY_K) {
        if (action == GLFW_PRESS) visualizer->keyK_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyK_ = false;
    }
    else if (key == GLFW_KEY_L) {
        if (action == GLFW_PRESS) visualizer->keyL_ = true;
        else if (action == GLFW_RELEASE) visualizer->keyL_ = false;
    }
    // Toggle camera mode with 'C' key
    else if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        visualizer->toggleCameraMode();
    }
}

GLVisualizer::GLVisualizer(Simulation& simulation, unsigned int width, unsigned int height, const std::string& title)
    : keyW_(false),
      keyA_(false),
      keyS_(false),
      keyD_(false),
      keyK_(false),
      keyL_(false),
      window_(nullptr),
      simulation_(simulation),
      width_(width),
      height_(height),
      particleSegments_(30),
      directionX_(1.0f),
      directionY_(0.0f),
      rotationAngle_(0.0f),
      rotationSpeed_(0.1f),  // Rotation speed in radians per frame (about 3 degrees)
      moveSpeed_(0.15f),      // Movement speed
      mouseX_(0.0),
      mouseY_(0.0),
      cameraHeight_(5.0f),   // Height of camera above the map
      cameraFollowSpeed_(0.1f), // Camera follows at 10% of the distance per frame
      cameraPosition_(0.0, 0.0, cameraHeight_),
      cameraTarget_(0.0, 0.0, 0.0),
      useFollowCamera_(true) { // Enable follow camera by default
    
    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    
    // Set error callback
    glfwSetErrorCallback(errorCallback);
    
    // Create window
    window_ = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window_) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent(window_);
    
    // Initialize GLEW
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return;
    }
    
    // Initialize GLUT for text rendering
    int argc = 1;
    char *argv[1] = {(char*)"Something"};
    glutInit(&argc, argv);
    
    // Store this pointer for callbacks
    glfwSetWindowUserPointer(window_, this);
    
    // Set key callback
    glfwSetKeyCallback(window_, keyCallback);
    
    // Set up OpenGL
    if (!initGL()) {
        std::cerr << "Failed to initialize OpenGL" << std::endl;
        return;
    }
    
    // Set up cursor position callback for mouse position tracking
    // (we still track the mouse position even though we don't use it for direction)
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos) {
        GLVisualizer* visualizer = static_cast<GLVisualizer*>(glfwGetWindowUserPointer(window));
        visualizer->mouseX_ = xpos;
        visualizer->mouseY_ = ypos;
    });
    
    // Set framebuffer size callback to handle window resizing
    glfwSetFramebufferSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
        GLVisualizer* visualizer = static_cast<GLVisualizer*>(glfwGetWindowUserPointer(window));
        visualizer->width_ = width;
        visualizer->height_ = height;
        
        // Update viewport to cover the entire window
        glViewport(0, 0, width, height);
        
        // Update projection matrix based on the current view mode
        if (visualizer->useFollowCamera_) {
            visualizer->setupPerspective();
        } else {
            // Orthographic projection for the traditional view
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            float viewHeight = 10.0f;
            float viewWidth = viewHeight * aspectRatio;
            
            // Center the view
            glOrtho(-viewWidth/2, viewWidth/2, -viewHeight/2, viewHeight/2, -1.0, 1.0);
            
            // Return to modelview matrix
            glMatrixMode(GL_MODELVIEW);
        }
    });
    
    // Initialize obstacles
    initializeObstacles();
    
    // Place the particle in a valid starting position
    placeParticleInValidPosition();
    
    std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    
    std::cout << "Controls:" << std::endl;
    std::cout << "  - W, A, S, D: Move particle" << std::endl;
    std::cout << "  - K, L: Rotate torch left/right" << std::endl;
    std::cout << "  - C: Toggle between follow camera and top-down view" << std::endl;
    std::cout << "  - ESC: Exit" << std::endl;
}

void GLVisualizer::placeParticleInValidPosition() {
    Particle* centralParticle = simulation_.getCentralParticle();
    if (!centralParticle) return;
    
    // Define spawn positions for defender and attacker sides
    struct SpawnPosition {
        float x, y;
        std::string name;
    };
    
    // Calculate scaling factor for wider screens
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    float scaleX = aspectRatio; // Use normal aspect ratio scaling
    
    // Define the two main spawn positions
    const SpawnPosition spawnPositions[] = {
        {-2.0f * scaleX, 3.5f, "Defender"},  // Defender spawn (top)
        {0.0f, -3.5f, "Attacker"}   // Attacker spawn (bottom)
    };
    
    // Randomly select one of the spawn positions
    int spawnIndex = rand() % 2;
    const auto& selectedSpawn = spawnPositions[spawnIndex];
    
    // Place the particle at the selected spawn point
    centralParticle->setPosition(Vector3D(selectedSpawn.x, selectedSpawn.y, 0));
    centralParticle->setVelocity(Vector3D(0, 0, 0));
    
    std::cout << "Particle spawned at " << selectedSpawn.name << " position." << std::endl;
}

GLVisualizer::~GLVisualizer() {
    // Clean up GLFW
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool GLVisualizer::initGL() {
    // Set the viewport to cover the entire window
    glViewport(0, 0, width_, height_);
    
    // Set up the projection matrix based on the camera mode
    if (useFollowCamera_) {
        setupPerspective();
    } else {
        // Use orthographic projection for the traditional view
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        // Orthographic projection - use full window size
        // Adjust the view size to match the window aspect ratio
        float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
        
        // Use fixed height and adjust width based on aspect ratio
        float viewHeight = 10.0f;
        float viewWidth = viewHeight * aspectRatio;
        
        // Center the view
        glOrtho(-viewWidth/2, viewWidth/2, -viewHeight/2, viewHeight/2, -1.0, 1.0);
    }
    
    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Enable depth testing for 3D rendering
    if (useFollowCamera_) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    
    // Set clear color to very dark gray (almost black) for tactical map look
    glClearColor(0.05f, 0.05f, 0.07f, 1.0f);
    
    return true;
}

void GLVisualizer::setupPerspective() {
    // Set up perspective projection for the follow camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    // Calculate the aspect ratio
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    
    // Use a narrower field of view (45 degrees) for less distortion
    // Adjust near and far clipping planes to better fit our scene
    gluPerspective(45.0f, aspectRatio, 0.1f, 100.0f);
    
    // Return to modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

void GLVisualizer::updateCamera() {
    // Get the central particle
    Particle* centralParticle = simulation_.getCentralParticle();
    if (!centralParticle) return;
    
    // Get the current particle position
    Vector3D particlePos = centralParticle->getPosition();
    
    // Set the camera target directly to the particle's position (no interpolation)
    // This ensures the camera is always looking at the particle
    cameraTarget_.x = particlePos.x;
    cameraTarget_.y = particlePos.y;
    cameraTarget_.z = 0.0; // Keep target at ground level
    
    // Position the camera directly above the particle at a fixed height
    // No interpolation to prevent lag and distortion
    cameraPosition_.x = particlePos.x;
    cameraPosition_.y = particlePos.y;
    cameraPosition_.z = cameraHeight_;
}

void GLVisualizer::initializeObstacles() {
    // Clear any existing obstacles
    obstacles_.clear();
    
    // Create a map layout similar to the provided image
    // The map has two spawn areas (Defender and Attacker) and various named locations
    
    // Calculate aspect ratio for scaling
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    
    // Use the full aspect ratio for scaling to fill the window width
    float scaleX = aspectRatio;
    
    // Defender side (top of map)
    // Defender spawn area
    obstacles_.emplace_back(-2.0f * scaleX, 3.0f, 4.0f * scaleX, 0.2f);  // Horizontal wall below spawn
    obstacles_.emplace_back(-4.0f * scaleX, 2.5f, 0.2f, 1.0f);  // Left vertical wall
    obstacles_.emplace_back(0.0f, 2.5f, 0.2f, 1.0f);   // Right vertical wall
    
    // A Site (top right)
    obstacles_.emplace_back(3.0f * scaleX, 3.0f, 4.0f * scaleX, 0.2f);   // Horizontal wall below A site
    obstacles_.emplace_back(1.0f * scaleX, 2.0f, 0.2f, 2.0f);   // Left vertical wall
    obstacles_.emplace_back(3.0f * scaleX, 1.5f, 4.0f * scaleX, 0.2f);   // Bottom wall of A site
    
    // A Main
    obstacles_.emplace_back(3.0f * scaleX, 0.5f, 0.2f, 2.0f);   // Vertical wall for A Main
    
    // A Link
    obstacles_.emplace_back(1.5f * scaleX, 1.0f, 3.0f * scaleX, 0.2f);   // Horizontal wall for A Link
    
    // A Lobby
    obstacles_.emplace_back(3.0f * scaleX, -1.5f, 0.2f, 2.0f);  // Vertical wall for A Lobby
    
    // Mid Courtyard
    obstacles_.emplace_back(0.0f, 0.0f, 2.0f * scaleX, 2.0f);   // Center obstacle
    
    // Mid Top
    obstacles_.emplace_back(-1.5f * scaleX, 1.5f, 3.0f * scaleX, 0.2f);  // Horizontal wall for Mid Top
    
    // B Site (top left)
    obstacles_.emplace_back(-3.0f * scaleX, 1.5f, 2.0f * scaleX, 0.2f);  // Bottom wall of B site
    obstacles_.emplace_back(-2.0f * scaleX, 2.0f, 0.2f, 1.0f);  // Right vertical wall of B site
    
    // B Main
    obstacles_.emplace_back(-3.0f * scaleX, 0.0f, 0.2f, 3.0f);  // Vertical wall for B Main
    
    // B Market
    obstacles_.emplace_back(-1.5f * scaleX, -0.5f, 3.0f * scaleX, 0.2f); // Horizontal wall for B Market
    
    // Mid Tiles
    obstacles_.emplace_back(0.0f, -1.5f, 2.0f * scaleX, 0.2f);  // Horizontal wall for Mid Tiles
    
    // Mid Bottom
    obstacles_.emplace_back(-1.5f * scaleX, -2.0f, 3.0f * scaleX, 0.2f); // Horizontal wall for Mid Bottom
    
    // B Lobby
    obstacles_.emplace_back(-2.0f * scaleX, -2.5f, 0.2f, 1.0f); // Vertical wall for B Lobby
    
    // Attacker side (bottom of map)
    // Attacker spawn area
    obstacles_.emplace_back(0.0f, -3.0f, 4.0f * scaleX, 0.2f);  // Horizontal wall above spawn
}

bool GLVisualizer::checkObstacleCollision(float x, float y, float radius) {
    // Check if the particle collides with any obstacle
    for (const auto& obstacle : obstacles_) {
        // Calculate the closest point on the rectangle to the circle
        float closestX = std::max(obstacle.x - obstacle.width/2, std::min(x, obstacle.x + obstacle.width/2));
        float closestY = std::max(obstacle.y - obstacle.height/2, std::min(y, obstacle.y + obstacle.height/2));
        
        // Calculate the distance between the circle's center and the closest point
        float distanceX = x - closestX;
        float distanceY = y - closestY;
        float distanceSquared = distanceX * distanceX + distanceY * distanceY;
        
        // Use the exact radius for collision detection to avoid false positives
        float radiusSquared = radius * radius;
        
        // If the distance is less than the circle's radius, collision detected
        if (distanceSquared < radiusSquared) {
            return true;
        }
        
        // Special case: check if the particle is fully inside the obstacle
        // This prevents the particle from getting stuck inside obstacles
        if (x > obstacle.x - obstacle.width/2 && x < obstacle.x + obstacle.width/2 &&
            y > obstacle.y - obstacle.height/2 && y < obstacle.y + obstacle.height/2) {
            return true;
        }
    }
    
    return false;
}

void GLVisualizer::run() {
    // Main loop
    while (!glfwWindowShouldClose(window_)) {
        // Update direction based on keyboard input
        updateDirection();
        
        // Handle keyboard input
        handleKeyboardInput();
        
        // Update simulation
        update();
        
        // Update camera position if using follow camera
        if (useFollowCamera_) {
            updateCamera();
        }
        
        // Render
        render();
        
        // Swap buffers
        glfwSwapBuffers(window_);
        
        // Poll for events
        glfwPollEvents();
    }
}

void GLVisualizer::updateDirection() {
    // Update direction based on keyboard input instead of mouse
    
    // Get the central particle
    Particle* centralParticle = simulation_.getCentralParticle();
    if (!centralParticle) return;
    
    // Update rotation angle based on key states
    if (keyK_) {
        // Rotate counter-clockwise (left)
        rotationAngle_ += rotationSpeed_;
    }
    if (keyL_) {
        // Rotate clockwise (right)
        rotationAngle_ -= rotationSpeed_;
    }
    
    // Keep angle in [0, 2π) range
    while (rotationAngle_ >= 2.0f * M_PI) {
        rotationAngle_ -= 2.0f * M_PI;
    }
    while (rotationAngle_ < 0.0f) {
        rotationAngle_ += 2.0f * M_PI;
    }
    
    // Calculate direction vector from rotation angle
    directionX_ = std::cos(rotationAngle_);
    directionY_ = std::sin(rotationAngle_);
}

void GLVisualizer::handleKeyboardInput() {
    Particle* centralParticle = simulation_.getCentralParticle();
    if (!centralParticle) return;
    
    Vector3D position = centralParticle->getPosition();
    Vector3D velocity(0, 0, 0);
    
    // Apply movement based on key states - always use full speed
    if (keyW_) velocity.y += moveSpeed_;
    if (keyS_) velocity.y -= moveSpeed_;
    if (keyA_) velocity.x -= moveSpeed_;
    if (keyD_) velocity.x += moveSpeed_;
    
    // If no movement, explicitly set velocity to zero and return early
    if (velocity.x == 0 && velocity.y == 0) {
        centralParticle->setVelocity(Vector3D(0, 0, 0));
        return;
    }
    
    // Try moving in both directions separately to allow sliding along obstacles
    Vector3D newPosition = position;
    bool canMoveX = true;
    bool canMoveY = true;
    
    // Check X movement
    Vector3D xMovement = position;
    xMovement.x += velocity.x;
    if (checkObstacleCollision(xMovement.x, position.y, particleRadius_)) {
        canMoveX = false;
        
        // Find the maximum distance we can move without colliding
        float maxMove = 0;
        float step = velocity.x / 10.0f; // Divide the movement into 10 steps
        float testX = position.x;
        
        for (int i = 0; i < 10; i++) {
            testX += step;
            if (checkObstacleCollision(testX, position.y, particleRadius_)) {
                break;
            }
            maxMove += step;
        }
        
        // Apply the maximum allowed movement
        newPosition.x = position.x + maxMove * 0.9f; // 90% of max to be safe
    }
    
    // Check Y movement
    Vector3D yMovement = position;
    yMovement.y += velocity.y;
    if (checkObstacleCollision(position.x, yMovement.y, particleRadius_)) {
        canMoveY = false;
        
        // Find the maximum distance we can move without colliding
        float maxMove = 0;
        float step = velocity.y / 10.0f; // Divide the movement into 10 steps
        float testY = position.y;
        
        for (int i = 0; i < 10; i++) {
            testY += step;
            if (checkObstacleCollision(position.x, testY, particleRadius_)) {
                break;
            }
            maxMove += step;
        }
        
        // Apply the maximum allowed movement
        newPosition.y = position.y + maxMove * 0.9f; // 90% of max to be safe
    }
    
    // Apply valid movements
    if (canMoveX) {
        newPosition.x = xMovement.x;
    }
    
    if (canMoveY) {
        newPosition.y = yMovement.y;
    }
    
    // Double-check combined movement (diagonal cases)
    if (canMoveX && canMoveY) {
        // Check diagonal movement
        if (checkObstacleCollision(newPosition.x, newPosition.y, particleRadius_)) {
            // Try to maintain at least one direction of movement
            if (!checkObstacleCollision(newPosition.x, position.y, particleRadius_)) {
                // Can move in X direction
                newPosition.y = position.y;
            } else if (!checkObstacleCollision(position.x, newPosition.y, particleRadius_)) {
                // Can move in Y direction
                newPosition.x = position.x;
            } else {
                // Can't move in either direction
                newPosition = position;
            }
        }
    }
    
    // Apply boundary constraints using the full view width
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    float viewHeight = 10.0f;
    float viewWidth = viewHeight * aspectRatio; // Use normal aspect ratio scaling
    
    newPosition.x = std::max(static_cast<double>(-viewWidth/2 + particleRadius_), 
                  std::min(static_cast<double>(viewWidth/2 - particleRadius_), newPosition.x));
    newPosition.y = std::max(static_cast<double>(-viewHeight/2 + particleRadius_), 
                  std::min(static_cast<double>(viewHeight/2 - particleRadius_), newPosition.y));
    
    // Update particle
    centralParticle->setPosition(newPosition);
    centralParticle->setVelocity(velocity);
}

void GLVisualizer::update() {
    // Get the central particle
    Particle* centralParticle = simulation_.getCentralParticle();
    if (!centralParticle) return;
    
    // Store current position before updating
    Vector3D oldPosition = centralParticle->getPosition();
    
    // Update the simulation
    simulation_.step(0.01); // 0.01 seconds per step
    
    // Get new position after update
    Vector3D newPosition = centralParticle->getPosition();
    
    // If the particle has moved through an obstacle, prevent that movement
    if (checkObstacleCollision(newPosition.x, newPosition.y, particleRadius_)) {
        // First try to maintain one axis of movement
        bool canMoveX = !checkObstacleCollision(newPosition.x, oldPosition.y, particleRadius_);
        bool canMoveY = !checkObstacleCollision(oldPosition.x, newPosition.y, particleRadius_);
        
        if (canMoveX) {
            // Can move along X axis
            newPosition.y = oldPosition.y;
        } else if (canMoveY) {
            // Can move along Y axis
            newPosition.x = oldPosition.x;
        } else {
            // Can't move in either direction, stay at old position
            newPosition = oldPosition;
        }
        
        // Update the position to prevent moving through obstacles
        centralParticle->setPosition(newPosition);
    }
    
    // If no keys are pressed, ensure the particle doesn't move at all
    if (!keyW_ && !keyA_ && !keyS_ && !keyD_) {
        // Reset velocity to zero but keep the current position
        centralParticle->setVelocity(Vector3D(0, 0, 0));
    }
}

void GLVisualizer::render() {
    // Clear the color and depth buffer
    if (useFollowCamera_) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
        glClear(GL_COLOR_BUFFER_BIT);
    }
    
    // Reset the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Set up the camera view if using follow camera
    if (useFollowCamera_) {
        // Set up the camera view using gluLookAt
        // Use a fixed up vector (0,1,0) to prevent tilting
        gluLookAt(
            cameraPosition_.x, cameraPosition_.y, cameraPosition_.z, // Camera position
            cameraTarget_.x, cameraTarget_.y, cameraTarget_.z,       // Look at target
            0.0f, 1.0f, 0.0f                                        // Up vector (fixed)
        );
        
        // Enable depth testing for proper 3D rendering
        glEnable(GL_DEPTH_TEST);
    } else {
        // Disable depth testing for 2D rendering
        glDisable(GL_DEPTH_TEST);
    }
    
    // Get the central particle
    Particle* centralParticle = simulation_.getCentralParticle();
    if (centralParticle) {
        // Get particle properties
        const Vector3D& position = centralParticle->getPosition();
        
        // Draw the grid overlay
        drawGrid();
        
        // Draw the obstacles
        glColor3f(0.5f, 0.5f, 0.5f); // Gray color for obstacles
        for (const auto& obstacle : obstacles_) {
            drawRectangle(obstacle.x, obstacle.y, obstacle.width, obstacle.height);
        }
        
        // Draw location labels
        drawLocationLabels();
        
        // Draw the direction torch (behind the particle)
        drawTorch(position.x, position.y, directionX_, directionY_, particleRadius_ * 1.5f);
        
        // Draw the particle as a solid ball
        glColor3f(1.0f, 0.2f, 0.2f); // Bright red color
        drawCircle(position.x, position.y, particleRadius_, particleSegments_);
    } else {
        // Draw the grid overlay
        drawGrid();
        
        // Draw the obstacles
        glColor3f(0.5f, 0.5f, 0.5f); // Gray color for obstacles
        for (const auto& obstacle : obstacles_) {
            drawRectangle(obstacle.x, obstacle.y, obstacle.width, obstacle.height);
        }
        
        // Draw site markers
        drawSiteMarkers();
        
        // Draw location labels
        drawLocationLabels();
    }
}

void GLVisualizer::drawCircle(float x, float y, float radius, int segments) {
    // In 3D mode, we need to draw at z=0 to be visible on the ground plane
    float z = 0.01f; // Slightly above the ground to avoid z-fighting
    
    glBegin(GL_TRIANGLE_FAN);
    
    // Center vertex
    if (useFollowCamera_) {
        glVertex3f(x, y, z);
    } else {
        glVertex2f(x, y);
    }
    
    // Circle vertices
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / static_cast<float>(segments);
        float vx = x + radius * std::cos(angle);
        float vy = y + radius * std::sin(angle);
        
        if (useFollowCamera_) {
            glVertex3f(vx, vy, z);
        } else {
            glVertex2f(vx, vy);
        }
    }
    
    glEnd();
}

void GLVisualizer::drawRectangle(float x, float y, float width, float height) {
    // In 3D mode, we need to draw at z=0 to be visible on the ground plane
    float z = 0.0f;
    
    glBegin(GL_QUADS);
    if (useFollowCamera_) {
        glVertex3f(x - width/2, y - height/2, z);
        glVertex3f(x + width/2, y - height/2, z);
        glVertex3f(x + width/2, y + height/2, z);
        glVertex3f(x - width/2, y + height/2, z);
    } else {
        glVertex2f(x - width/2, y - height/2);
        glVertex2f(x + width/2, y - height/2);
        glVertex2f(x + width/2, y + height/2);
        glVertex2f(x - width/2, y + height/2);
    }
    glEnd();
}

 

// Add this after the drawArrow method
void GLVisualizer::drawText(const std::string& text, float x, float y) {
    // Simple implementation to draw text at the specified position
    // This is a basic implementation - for production code, consider using a proper text rendering library
    glRasterPos2f(x, y);
    for (const char& c : text) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
    }
}

// Add this new method to draw all location labels
void GLVisualizer::drawLocationLabels() {
    // Set text color to white
    glColor3f(1.0f, 1.0f, 1.0f);
    
    // Calculate scaling factor for wider screens
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    float scaleX = aspectRatio; // Use normal aspect ratio scaling
    
    // Draw labels for all locations
    drawText("DEFENDER SIDE SPAWN", -2.0f * scaleX, 3.5f);
    drawText("A SITE", 3.0f * scaleX, 2.5f);
    drawText("A ELBOW", 4.0f * scaleX, 1.0f);
    drawText("A LINK", 1.5f * scaleX, 1.3f);
    drawText("A MAIN", 3.5f * scaleX, 0.0f);
    drawText("A LOBBY", 3.5f * scaleX, -2.0f);
    drawText("MID COURTYARD", 0.0f, 0.3f);
    drawText("MID TOP", -1.5f * scaleX, 1.8f);
    drawText("MID TILES", 0.0f, -1.8f);
    drawText("MID BOTTOM", -1.5f * scaleX, -2.3f);
    drawText("B SITE", -3.0f * scaleX, 2.5f);
    drawText("B BOBA", -3.5f * scaleX, 1.0f);
    drawText("B MAIN", -3.5f * scaleX, -1.0f);
    drawText("B MARKET", -1.5f * scaleX, -0.8f);
    drawText("B LOBBY", -2.5f * scaleX, -2.8f);
    drawText("ATTACKER SIDE SPAWN", 0.0f, -3.5f);
} 

// Add this after drawLocationLabels method
void GLVisualizer::drawSiteMarkers() {
    // Draw site markers (A and B) as red circles with labels
    
    // Calculate scaling factor for wider screens
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    float scaleX = aspectRatio; // Use normal aspect ratio scaling
    
    // A Site marker
    glColor3f(1.0f, 0.3f, 0.3f); // Red color for the marker
    drawCircle(3.5f * scaleX, 2.0f, 0.3f, 20); // Position for A site
    
    // B Site marker
    glColor3f(1.0f, 0.3f, 0.3f); // Red color for the marker
    drawCircle(-3.0f * scaleX, 2.0f, 0.3f, 20); // Position for B site
    
    // Draw the A and B labels inside the markers
    glColor3f(1.0f, 1.0f, 1.0f); // White color for text
    drawText("A", 3.45f * scaleX, 1.95f);
    drawText("B", -3.05f * scaleX, 1.95f);
} 

// Add this after drawSiteMarkers method
void GLVisualizer::drawGrid() {
    // Draw a grid overlay to make the map look more tactical
    
    // Set grid color to dark gray
    glColor4f(0.2f, 0.2f, 0.2f, 0.3f); // Semi-transparent dark gray
    
    // Set line width
    glLineWidth(1.0f);
    
    // Calculate grid size based on view dimensions
    float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
    float viewHeight = 10.0f;
    float viewWidth = viewHeight * aspectRatio; // Use normal aspect ratio scaling
    
    // Grid cell size
    float gridSize = 0.5f;
    
    // Z coordinate for 3D mode (slightly above ground)
    float z = 0.0f;
    
    // Draw vertical grid lines
    glBegin(GL_LINES);
    for (float x = -viewWidth/2; x <= viewWidth/2; x += gridSize) {
        if (useFollowCamera_) {
            glVertex3f(x, -viewHeight/2, z);
            glVertex3f(x, viewHeight/2, z);
        } else {
            glVertex2f(x, -viewHeight/2);
            glVertex2f(x, viewHeight/2);
        }
    }
    
    // Draw horizontal grid lines
    for (float y = -viewHeight/2; y <= viewHeight/2; y += gridSize) {
        if (useFollowCamera_) {
            glVertex3f(-viewWidth/2, y, z);
            glVertex3f(viewWidth/2, y, z);
        } else {
            glVertex2f(-viewWidth/2, y);
            glVertex2f(viewWidth/2, y);
        }
    }
    glEnd();
} 

void GLVisualizer::drawTorch(float x, float y, float dirX, float dirY, float length) {
    // Create a torch light effect with improved physics-based bending around obstacles
    // and enhanced visual effects for smoother appearance
    
    // Torch parameters
    const int numRays = 80;        // Increased ray density for smoother appearance
    const float coneAngle = M_PI / 3.8f; // Slightly narrower cone for better control
    const float torchLength = length * 3.5f; // Slightly longer torch for better effect
    const int bendSteps = 30;      // Increased steps for smoother bending
    
    // Calculate the base angle
    float baseAngle = std::atan2(dirY, dirX);
    
    // Enable blending for the light effect
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Draw an outer glow around the particle
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 0.6f, 0.0f, 0.7f); // Center color (orange)
    glVertex2f(x, y);
    
    const float glowRadius = particleRadius_ * 1.5f;
    const int glowSegments = 32; // Increased for smoother glow
    for (int i = 0; i <= glowSegments; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / glowSegments;
        glColor4f(1.0f, 0.3f, 0.0f, 0.0f); // Outer color (transparent)
        float vx = x + glowRadius * std::cos(angle);
        float vy = y + glowRadius * std::sin(angle);
        glVertex2f(vx, vy);
    }
    glEnd();
    
    // Store all ray paths for density calculation
    std::vector<std::vector<std::pair<float, float>>> allRayPaths;
    
    // First pass: Calculate all ray paths with improved physics
    for (int i = 0; i <= numRays; ++i) {
        float ratio = static_cast<float>(i) / numRays;
        float angle = baseAngle - coneAngle/2.0f + coneAngle * ratio;
        
        // Initial ray direction
        float rayDirX = std::cos(angle);
        float rayDirY = std::sin(angle);
        
        // Physics parameters for this ray
        float rayLength = torchLength * (0.85f + 0.3f * std::sin(ratio * M_PI)); // More consistent length
        float stepSize = rayLength / bendSteps;
        
        // Start at particle position
        float currentX = x;
        float currentY = y;
        
        // Store the path of the ray for drawing
        std::vector<std::pair<float, float>> rayPath;
        rayPath.push_back({currentX, currentY});
        
        // Previous direction for smoothing
        float prevDirX = rayDirX;
        float prevDirY = rayDirY;
        
        // Simulate light bending using an improved physics model
        for (int step = 0; step < bendSteps; ++step) {
            // Calculate forces from nearby obstacles with improved physics
            float forceX = 0.0f;
            float forceY = 0.0f;
            
            // Distance-based influence factor (rays travel further before bending)
            float distanceFactor = 1.0f - static_cast<float>(step) / bendSteps * 0.5f;
            
            // Check all obstacles for repulsive forces with improved physics
            for (const auto& obstacle : obstacles_) {
                // Calculate center and boundaries of obstacle
                float obstacleLeft = obstacle.x - obstacle.width/2;
                float obstacleRight = obstacle.x + obstacle.width/2;
                float obstacleTop = obstacle.y + obstacle.height/2;
                float obstacleBottom = obstacle.y - obstacle.height/2;
                
                // Calculate distance to each edge of the obstacle
                float distToLeft = currentX - obstacleLeft;
                float distToRight = obstacleRight - currentX;
                float distToTop = obstacleTop - currentY;
                float distToBottom = currentY - obstacleBottom;
                
                // Check if point is near the obstacle
                bool nearObstacle = 
                    currentX >= obstacleLeft - 1.0f && currentX <= obstacleRight + 1.0f &&
                    currentY >= obstacleBottom - 1.0f && currentY <= obstacleTop + 1.0f;
                
                if (!nearObstacle) continue;
                
                // Find the closest edge
                float minDist = std::min({distToLeft, distToRight, distToTop, distToBottom});
                
                // Apply smoother distance-based force
                if (minDist > 0.01f && minDist < 2.0f) {
                    // Improved inverse square law with smoother falloff
                    float repulsiveForce = 0.08f / (minDist * minDist + 0.1f) * distanceFactor;
                    
                    // Calculate normal vector from obstacle
                    float normalX = 0.0f;
                    float normalY = 0.0f;
                    
                    // Determine which edge is closest and calculate normal
                    if (minDist == distToLeft) {
                        normalX = -1.0f;
                    } else if (minDist == distToRight) {
                        normalX = 1.0f;
                    } else if (minDist == distToTop) {
                        normalY = 1.0f;
                    } else if (minDist == distToBottom) {
                        normalY = -1.0f;
                    }
                    
                    // Apply force in normal direction
                    forceX += normalX * repulsiveForce;
                    forceY += normalY * repulsiveForce;
                    
                    // Add corner handling for smoother bends
                    // Check if we're near a corner
                    const float cornerThreshold = 0.5f;
                    bool nearTopLeft = (distToLeft < cornerThreshold && distToTop < cornerThreshold);
                    bool nearTopRight = (distToRight < cornerThreshold && distToTop < cornerThreshold);
                    bool nearBottomLeft = (distToLeft < cornerThreshold && distToBottom < cornerThreshold);
                    bool nearBottomRight = (distToRight < cornerThreshold && distToBottom < cornerThreshold);
                    
                    if (nearTopLeft || nearTopRight || nearBottomLeft || nearBottomRight) {
                        // Enhanced corner force
                        float cornerForce = 0.15f / (minDist * minDist + 0.05f) * distanceFactor;
                        
                        // Apply appropriate corner normal
                        if (nearTopLeft) {
                            forceX -= cornerForce * 0.7071f; // -cos(45°)
                            forceY += cornerForce * 0.7071f; // sin(45°)
                        } else if (nearTopRight) {
                            forceX += cornerForce * 0.7071f;
                            forceY += cornerForce * 0.7071f;
                        } else if (nearBottomLeft) {
                            forceX -= cornerForce * 0.7071f;
                            forceY -= cornerForce * 0.7071f;
                        } else if (nearBottomRight) {
                            forceX += cornerForce * 0.7071f;
                            forceY -= cornerForce * 0.7071f;
                        }
                    }
                }
            }
            
            // Apply the forces to bend the ray direction with smoothing
            float bendFactor = 0.25f; // Increased for more responsive bending
            rayDirX += forceX * bendFactor;
            rayDirY += forceY * bendFactor;
            
            // Apply smoothing between previous and current direction (prevents jittering)
            float smoothFactor = 0.7f;
            rayDirX = prevDirX * smoothFactor + rayDirX * (1.0f - smoothFactor);
            rayDirY = prevDirY * smoothFactor + rayDirY * (1.0f - smoothFactor);
            
            // Store previous direction
            prevDirX = rayDirX;
            prevDirY = rayDirY;
            
            // Normalize the direction vector
            float dirLength = std::sqrt(rayDirX * rayDirX + rayDirY * rayDirY);
            if (dirLength > 0.001f) {
                rayDirX /= dirLength;
                rayDirY /= dirLength;
            }
            
            // Move along the ray
            float nextX = currentX + rayDirX * stepSize;
            float nextY = currentY + rayDirY * stepSize;
            
            // Check if the ray hits an obstacle
            bool hitObstacle = false;
            for (const auto& obstacle : obstacles_) {
                if (nextX >= obstacle.x - obstacle.width/2 && nextX <= obstacle.x + obstacle.width/2 &&
                    nextY >= obstacle.y - obstacle.height/2 && nextY <= obstacle.y + obstacle.height/2) {
                    hitObstacle = true;
                    break;
                }
            }
            
            if (hitObstacle) {
                // Stop the ray if it hits an obstacle
                break;
            }
            
            // Update current position
            currentX = nextX;
            currentY = nextY;
            rayPath.push_back({currentX, currentY});
        }
        
        // Store this ray path
        allRayPaths.push_back(rayPath);
    }
    
    // Second pass: Draw solid connections between adjacent rays with improved smoothing
    for (size_t i = 0; i < allRayPaths.size() - 1; ++i) {
        const auto& path1 = allRayPaths[i];
        const auto& path2 = allRayPaths[i + 1];
        
        // Skip if either path is too short
        if (path1.size() < 2 || path2.size() < 2) continue;
        
        // Calculate the minimum length to use for the quad strips
        size_t minLength = std::min(path1.size(), path2.size());
        
        // Draw a quad strip between the two rays
        glBegin(GL_QUAD_STRIP);
        
        for (size_t j = 0; j < minLength; ++j) {
            // Calculate progress along the ray (0 at source, 1 at end)
            float t = static_cast<float>(j) / (minLength - 1);
            
            // Calculate position in the cone (0 at left edge, 1 at right edge)
            float rayPosition = static_cast<float>(i) / (allRayPaths.size() - 2);
            
            // Calculate alpha based on position with improved falloff
            // Higher in the center of the cone, lower at edges
            float centerFactor = 1.0f - std::pow(std::abs(rayPosition - 0.5f) * 2.0f, 1.5f);
            
            // Improved distance falloff with smoother gradient
            float distanceFactor = 1.0f - std::pow(t, 1.2f) * 0.8f;
            
            // Combine factors for final alpha with improved smoothing
            float alpha = std::max(0.0f, 0.85f * centerFactor * distanceFactor);
            
            // Improved color gradient from yellow/orange to red with distance
            float r = 1.0f;
            float g = 0.9f - 0.7f * std::pow(t, 1.2f);
            float b = 0.25f * (1.0f - t) * centerFactor;
            
            // Add vertices for the quad strip
            glColor4f(r, g, b, alpha);
            glVertex2f(path1[j].first, path1[j].second);
            glVertex2f(path2[j].first, path2[j].second);
        }
        
        glEnd();
    }
    
    // Third pass: Draw ray outlines (fewer, more subtle)
    if (numRays > 10) {
        // Only draw some of the rays for outline effect (more sparse for cleaner look)
        for (size_t i = 0; i < allRayPaths.size(); i += 15) {
            const auto& rayPath = allRayPaths[i];
            
            // Draw the ray as a thin line
            if (rayPath.size() > 1) {
                glLineWidth(0.8f); // Thinner lines for subtler effect
                glBegin(GL_LINE_STRIP);
                
                // Start with full brightness at the particle
                glColor4f(1.0f, 0.9f, 0.4f, 0.2f); // More transparent for subtler effect
                glVertex2f(rayPath[0].first, rayPath[0].second);
                
                // Draw the ray path with gradient
                for (size_t j = 1; j < rayPath.size(); ++j) {
                    float t = static_cast<float>(j) / rayPath.size();
                    float alpha = 0.2f * (1.0f - t * t); // Very transparent
                    glColor4f(1.0f, 0.8f - 0.6f * t, 0.0f, alpha);
                    glVertex2f(rayPath[j].first, rayPath[j].second);
                }
                
                glEnd();
            }
        }
    }
    
    // Draw volumetric particles in the light cone for additional density
    static std::mt19937 gen(static_cast<unsigned int>(time(nullptr)));
    std::uniform_real_distribution<float> coneAngleDist(-coneAngle/2, coneAngle/2);
    std::uniform_real_distribution<float> distDist(0.2f, 0.9f);
    std::uniform_real_distribution<float> sizeDist(0.02f, 0.07f); // Slightly smaller particles
    std::uniform_real_distribution<float> alphaDist(0.1f, 0.4f); // Slightly more transparent
    
    // Add volumetric particles throughout the cone for additional density
    const int numVolumetricParticles = 60; // More particles for density
    for (int i = 0; i < numVolumetricParticles; ++i) {
        // Random position within the cone
        float particleAngle = baseAngle + coneAngleDist(gen);
        float distance = distDist(gen) * torchLength;
        float px = x + std::cos(particleAngle) * distance;
        float py = y + std::sin(particleAngle) * distance;
        
        // Check if particle is inside an obstacle
        bool insideObstacle = false;
        for (const auto& obstacle : obstacles_) {
            if (px >= obstacle.x - obstacle.width/2 && px <= obstacle.x + obstacle.width/2 &&
                py >= obstacle.y - obstacle.height/2 && py <= obstacle.y + obstacle.height/2) {
                insideObstacle = true;
                break;
            }
        }
        
        // Skip if inside obstacle
        if (insideObstacle) continue;
        
        // Random size and color
        float size = sizeDist(gen) * 0.8f;
        float distanceRatio = distance / torchLength;
        float alpha = alphaDist(gen) * (1.0f - distanceRatio * 0.7f);
        
        // Draw volumetric particle
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.7f, 0.2f, alpha);
        glVertex2f(px, py);
        
        const int particleSegments = 6;
        for (int j = 0; j <= particleSegments; ++j) {
            float particleAngle = 2.0f * M_PI * j / particleSegments;
            float vx = px + size * std::cos(particleAngle);
            float vy = py + size * std::sin(particleAngle);
            glColor4f(1.0f, 0.5f, 0.0f, 0.0f);
            glVertex2f(vx, vy);
        }
        glEnd();
    }
    
    // Draw flame particles at the center
    std::uniform_real_distribution<float> angleDist(-M_PI, M_PI);
    std::uniform_real_distribution<float> radiusDist(0.05f, particleRadius_ * 0.8f);
    
    const int numParticles = 15; // More particles for denser effect
    for (int i = 0; i < numParticles; ++i) {
        // Random position near the center
        float angle = angleDist(gen);
        float radius = radiusDist(gen);
        float px = x + radius * std::cos(angle);
        float py = y + radius * std::sin(angle);
        
        // Random size and color
        float size = sizeDist(gen);
        
        // Draw flame particle
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(1.0f, 0.9f, 0.3f, 0.8f);
        glVertex2f(px, py);
        
        const int particleSegments = 8;
        for (int j = 0; j <= particleSegments; ++j) {
            float particleAngle = 2.0f * M_PI * j / particleSegments;
            float vx = px + size * std::cos(particleAngle);
            float vy = py + size * std::sin(particleAngle);
            glColor4f(1.0f, 0.5f, 0.0f, 0.0f);
            glVertex2f(vx, vy);
        }
        glEnd();
    }
    
    // Add a bright center at the particle position
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(1.0f, 1.0f, 0.7f, 0.95f);
    glVertex2f(x, y);
    
    const float centerRadius = particleRadius_ * 0.6f;
    for (int i = 0; i <= 16; ++i) {
        float angle = 2.0f * M_PI * static_cast<float>(i) / 16.0f;
        float vx = x + centerRadius * std::cos(angle);
        float vy = y + centerRadius * std::sin(angle);
        glColor4f(1.0f, 0.8f, 0.2f, 0.0f);
        glVertex2f(vx, vy);
    }
    glEnd();
    
    // Reset line width
    glLineWidth(1.0f);
    glDisable(GL_BLEND);
} 

void GLVisualizer::toggleCameraMode() {
    // Toggle between follow camera and orthographic view
    useFollowCamera_ = !useFollowCamera_;
    
    // Update the OpenGL settings for the new mode
    if (useFollowCamera_) {
        // Enable depth testing for 3D
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        
        // Set up perspective projection
        setupPerspective();
        
        std::cout << "Camera mode: Follow camera (3D perspective)" << std::endl;
    } else {
        // Disable depth testing for 2D
        glDisable(GL_DEPTH_TEST);
        
        // Set up orthographic projection
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        float aspectRatio = static_cast<float>(width_) / static_cast<float>(height_);
        float viewHeight = 10.0f;
        float viewWidth = viewHeight * aspectRatio;
        
        // Center the view
        glOrtho(-viewWidth/2, viewWidth/2, -viewHeight/2, viewHeight/2, -1.0, 1.0);
        
        // Return to modelview matrix
        glMatrixMode(GL_MODELVIEW);
        
        std::cout << "Camera mode: Top-down view (2D orthographic)" << std::endl;
    }
} 