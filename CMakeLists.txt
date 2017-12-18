cmake_minimum_required(VERSION 3.6)
project(SpacInvaders)

set(CMAKE_CXX_STANDARD 11)

# Windows mingw linker options
#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -mwindows")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
# Show all makefile commands
#set(CMAKE_VERBOSE_MAKEFILE ON)

find_package(OpenGL REQUIRED)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(libs/glfw-3.2.1)
include_directories(libs/glfw-3.2.1/include)

add_subdirectory(libs/freetype-2.7)
include_directories(libs/freetype-2.7/include)

add_subdirectory(libs/gl3w-master)
include_directories(libs/gl3w-master/include)

add_subdirectory(libs/glm-0.9.9-a2)
include_directories(libs/glm-0.9.9-a2/glm)

# Windows Threading lib
# include_directories(libs/mingw-std-threads-master)

# Copy Resources
file(COPY shaders DESTINATION ./)
file(COPY fonts DESTINATION ./)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DSI_DEBUG)
endif()


set(SOURCE_FILES main.cpp shaderManager.cpp State.cpp)
add_executable(SpacInvaders ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/resources/meta/Program.rc)

target_link_libraries(SpacInvaders glfw ${GLFW_LIBRARIES} gl3w ${OPENGL_gl_LIBRARY} freetype glm)
