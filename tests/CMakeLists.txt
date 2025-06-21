# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add test executable
add_executable(
  physics_tests
  test_main.cpp
  ${CMAKE_SOURCE_DIR}/src/particle.cpp
  ${CMAKE_SOURCE_DIR}/src/simulation.cpp
)

# Link against gtest libraries
target_link_libraries(
  physics_tests
  GTest::gtest_main
)

# Register tests
include(GoogleTest)
gtest_discover_tests(physics_tests) 