cmake_minimum_required(VERSION 3.16)

PROJECT(Particle)
option(SDL_SHARED "" OFF)
option(SDL_STATIC "" ON)
add_subdirectory(libs/vendored/SDL)
add_subdirectory(libs/geGL-master)


find_package(glm REQUIRED)

set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui-master)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/scene.cpp
    src/particle.cpp
    src/camera.cpp
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC SDL3::SDL3 geGL::geGL glm::glm)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

add_custom_target(run ./${PROJECT_NAME})