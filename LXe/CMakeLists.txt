
cmake_minimum_required(VERSION 3.16...3.21)
project(LXe)

find_package(Geant4 REQUIRED)

# Include directories
include(${Geant4_USE_FILE})

# Sources
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# Executable
add_executable(LXe ${sources} ${headers})

# Include directories
target_include_directories(LXe PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Link to Geant4 libraries
target_link_libraries(LXe PUBLIC ${Geant4_LIBRARIES})
