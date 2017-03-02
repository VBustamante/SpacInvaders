cmake_minimum_required(VERSION 3.6)
project(SpacInvaders)

set(CMAKE_CXX_STANDARD 11)

# Windows mingw linker options
#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -mwindows")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
# Show all makefile commands
set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.2.1)
include_directories(libs/glfw-3.2.1/include)

add_subdirectory(libs/gl3w-master)
include_directories(libs/gl3w-master/include)

# Copy Resources
file(COPY shaders DESTINATION ./)


set(SOURCE_FILES main.cpp)
add_executable(SpacInvaders ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/resources/Program.rc)

target_link_libraries(SpacInvaders glfw ${GLFW_LIBRARIES} gl3w ${OPENGL_gl_LIBRARY})
