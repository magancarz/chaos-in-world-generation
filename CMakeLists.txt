cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 23)

project(chaos-in-world-generation LANGUAGES C CXX)

file(GLOB_RECURSE CIWG_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp)
add_library(chaos-in-world-generation-lib STATIC ${CIWG_LIB_SOURCES})

target_include_directories(chaos-in-world-generation-lib PUBLIC Include)
target_include_directories(chaos-in-world-generation-lib PUBLIC ThirdParty)

add_executable(chaos-in-world-generation Application/main.cpp)
target_link_libraries(chaos-in-world-generation
    chaos-in-world-generation-lib)