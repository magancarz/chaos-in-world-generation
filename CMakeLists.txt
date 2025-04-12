cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)

project(chaos-in-world-generation LANGUAGES C CXX)

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
link_libraries(${GLEW_LIBRARIES})

find_package(OpenGL REQUIRED)

file(GLOB_RECURSE CIWG_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
add_library(chaos-in-world-generation-lib STATIC ${CIWG_LIB_SOURCES})

add_subdirectory(ThirdParty)

target_link_libraries(chaos-in-world-generation-lib
    OpenGL::GL
    glfw
    glm::glm)

target_include_directories(chaos-in-world-generation-lib PUBLIC Include)
target_include_directories(chaos-in-world-generation-lib PUBLIC ThirdParty)

add_executable(chaos-in-world-generation Application/main.cpp)
target_link_libraries(chaos-in-world-generation
    chaos-in-world-generation-lib)