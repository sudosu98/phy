cmake_minimum_required(VERSION 3.10)
project(PhysicsSimulation VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLEW
find_package(GLEW REQUIRED)
if(APPLE)
    include_directories(/opt/homebrew/include)
    link_directories(/opt/homebrew/lib)
endif()

# Find GLFW3
find_package(glfw3 REQUIRED)

# Find GLUT
find_package(GLUT REQUIRED)

# Include directories
include_directories(include ${OPENGL_INCLUDE_DIR} ${GLEW_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Add the executable
add_executable(simulation 
    src/main.cpp
    src/particle.cpp
    src/simulation.cpp
    src/gl_visualizer.cpp
)

# Link OpenGL libraries
target_link_libraries(simulation PRIVATE 
    ${OPENGL_LIBRARIES}
    GLEW::GLEW
    glfw
    ${GLUT_LIBRARIES}
)

# Enable testing
enable_testing()
add_subdirectory(tests)

# Set build type to Debug by default
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif()