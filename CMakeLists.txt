cmake_minimum_required(VERSION 3.25.0)
project(VoxelEngine VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
endif()
# Find SDL2
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)
# Add the executable target
add_executable(VoxelEngine voxelEngine.c dos/dos.c)

# Include SDL2 headers
target_include_directories(VoxelEngine PRIVATE ${SDL2_INCLUDE_DIRS})

# Link SDL2 library
target_link_libraries(VoxelEngine PRIVATE ${SDL2_LIBRARIES} GLEW::GLEW winmm)
