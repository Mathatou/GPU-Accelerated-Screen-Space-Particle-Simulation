cmake_minimum_required(VERSION 3.16)

PROJECT(Particle)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(libs/vendored/SDL)
add_subdirectory(libs/geGL-master)


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 geGL::geGL)
add_custom_target(run ./${PROJECT_NAME})