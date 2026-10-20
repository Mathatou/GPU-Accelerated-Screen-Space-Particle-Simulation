cmake_minimum_required(VERSION 3.16)

PROJECT(Particle)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(libs/vendored/SDL)
add_subdirectory(libs/geGL-master)

find_package(glm REQUIRED)


add_executable(${PROJECT_NAME} src/main.cpp src/scene.cpp src/particle.cpp src/camera.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 geGL::geGL glm::glm)
add_custom_target(run ./${PROJECT_NAME})