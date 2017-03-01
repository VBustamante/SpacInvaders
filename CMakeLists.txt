cmake_minimum_required(VERSION 3.6)
project(SpacInvaders)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_VERBOSE_MAKEFILE ON)
find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.2.1)
include_directories(libs/glfw-3.2.1/include)

add_subdirectory(libs/gl3w-master)
include_directories(libs/gl3w-master/include)

set(SOURCE_FILES main.cpp)
add_executable(SpacInvaders ${SOURCE_FILES})

target_link_libraries(SpacInvaders glfw ${GLFW_LIBRARIES} gl3w ${OPENGL_gl_LIBRARY})
