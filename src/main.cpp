#define GL_SILENCE_DEPRECATION // Silence OpenGL deprecation warnings on macOS

#include <iostream>
#include <memory>
#include <vector>
#include "simulation.hpp"
#include "gl_visualizer.hpp"

int main(int argc, char* argv[]) {
    std::cout << "OpenGL Physics Simulation Starting..." << std::endl;
    
    try {
        // Create simulation with default parameters
        auto simulation = std::make_unique<Simulation>();
        
        // Initialize the simulation
        simulation->initialize();
        
        // Create visualizer with a larger window size
        GLVisualizer visualizer(*simulation, 1280, 960, "OpenGL Physics Simulation");
        
        std::cout << "Controls:" << std::endl;
        std::cout << "  - W, A, S, D: Move particle" << std::endl;
        std::cout << "  - K, L: Rotate torch left/right" << std::endl;
        std::cout << "  - ESC: Exit" << std::endl;
        
        // Run the visualization (this will also run the simulation)
        visualizer.run();
        
        std::cout << "Simulation completed successfully." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 