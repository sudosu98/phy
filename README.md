# Physics Simulation Project

A C++ physics simulation project that models particle dynamics in 3D space with an OpenGL-based tactical map visualization.

## Features

- 3D vector mathematics
- Particle-based physics simulation
- OpenGL visualization with tactical map layout
- Physics-based torch light effect that bends around obstacles
- Keyboard controls for player movement and torch rotation
- Collision detection with map obstacles
- Location markers and labels for tactical navigation

## Project Structure

```
.
├── CMakeLists.txt          # Main CMake configuration
├── include/                # Header files
│   ├── vector3d.hpp        # 3D vector class
│   ├── particle.hpp        # Particle class
│   ├── simulation.hpp      # Simulation class
│   └── gl_visualizer.hpp   # OpenGL visualization class
├── src/                    # Source files
│   ├── main.cpp            # Main application entry point
│   ├── particle.cpp        # Particle implementation
│   ├── simulation.cpp      # Simulation implementation
│   └── gl_visualizer.cpp   # OpenGL visualization implementation
├── tests/                  # Test files
│   ├── CMakeLists.txt      # Test CMake configuration
│   └── test_main.cpp       # Test implementations
└── build/                  # Build directory (generated)
```

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler (GCC, Clang, MSVC)
- OpenGL libraries
- GLEW
- GLFW3
- GLUT

### Build Instructions

1. Create a build directory and navigate to it:

   ```bash
   mkdir -p build
   cd build
   ```

2. Generate the build files:

   ```bash
   cmake ..
   ```

3. Build the project:

   ```bash
   cmake --build .
   ```

4. Run the simulation:

   ```bash
   ./simulation
   ```

5. Run the tests:
   ```bash
   ctest
   ```

## Controls

- **W, A, S, D**: Move the player character
- **K, L**: Rotate the torch light left/right
- **ESC**: Exit the application

## Development Journey

This project evolved from a basic physics simulation to a tactical map visualization with advanced lighting effects. Here's a summary of the key development steps and decisions:

### Initial Setup

- Created a basic physics engine with vector mathematics and particle dynamics
- Set up OpenGL visualization with GLFW and GLEW for window management
- Implemented a simple orthographic projection for 2D visualization

### Tactical Map Implementation

- Designed a tactical game map layout with different named locations
- Added rectangular obstacles to create map boundaries and pathways
- Implemented spawn points for "Defender" and "Attacker" sides
- Created text rendering for location labels using GLUT
- Added site markers (A and B) for tactical reference points
- Implemented a grid overlay for a tactical map appearance

### Player Movement and Controls

- Changed from mouse-based to keyboard-based controls for better gameplay
- Implemented WASD movement with collision detection against obstacles
- Added sliding along obstacles when the player collides at an angle
- Implemented boundary constraints to keep the player within the map
- Added K and L keys for torch rotation control

### Torch Light Effect

- Created a physics-based torch light effect that bends around obstacles
- Implemented ray casting with multiple rays for a cone-shaped light
- Added physics simulation to make light rays bend around corners and obstacles
- Increased ray density for a more solid and realistic light effect
- Enhanced the visual appearance with color gradients and alpha blending
- Added volumetric particles for additional density and realism
- Implemented corner detection for smoother bending around obstacles

### Visual Enhancements

- Added a glowing effect around the player character
- Implemented a dark background for better contrast
- Created a grid overlay for tactical map appearance
- Added smooth color transitions in the torch light effect
- Implemented semi-transparent effects for more realistic lighting

### Performance Optimizations

- Used quad strips between adjacent rays for more efficient rendering
- Implemented selective ray outline drawing for better performance
- Added distance-based influence factors for more realistic light behavior
- Optimized collision detection for better performance

## License

This project is open source and available under the MIT License.
