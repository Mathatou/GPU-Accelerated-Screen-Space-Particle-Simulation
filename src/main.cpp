#pragma region Includes
#include "scene.hpp"

#include <iostream>
#include "../libs/imgui-master/imgui.h"
#include "../libs/imgui-master/backends/imgui_impl_sdl3.h"
#include "../libs/imgui-master/backends/imgui_impl_opengl3.h"

#include <map>
#pragma endregion
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define MAX_PARTICLE_NUMBER 1000000

#pragma region Application State
int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;
auto lTimer = SDL_GetPerformanceCounter();
auto lFrequency = SDL_GetPerformanceFrequency();
int frameCount = 0;
float FPS = 0.f;
bool FPSDebugMode = true;
bool isFullScreen = false;
bool isMouseRelative = true;
bool isColorManual = false;
Scene lScene;
std::map<int, bool> keys;
#pragma endregion

#pragma region Event Handling
void HandleEvent(const SDL_Event& pEvent, bool& running)
{
    if(pEvent.type == SDL_EVENT_QUIT)
    {
        running = false;
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_ESCAPE)
    {
        running = false;
        std::cout << "Escape key pressed. Exiting..." << std::endl;
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN)
    {
        keys[pEvent.key.key] = true;
        if(pEvent.key.key == SDLK_R)
        {
            lScene.lCamera.Reset();
        }
    }
    if(pEvent.type == SDL_EVENT_KEY_UP)
    {
        keys[pEvent.key.key] = false;
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F5)
    {
        std::cout << "Switching fps counter display mode." << std::endl;
        FPSDebugMode = !FPSDebugMode;
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F11)
    {
        if(isFullScreen)
        {
            std::cout << "Switching to windowed mode." << std::endl;
            isFullScreen = false;
        }
        else
        {
            std::cout << "Switching to fullscreen mode." << std::endl;
            isFullScreen = true;
        }

        Uint32 windowFlags = SDL_GetWindowFlags(SDL_GL_GetCurrentWindow());
        if(windowFlags & SDL_WINDOW_FULLSCREEN)
        {
            SDL_SetWindowFullscreen(SDL_GL_GetCurrentWindow(), 0);
        }
        else
        {
            SDL_SetWindowFullscreen(SDL_GL_GetCurrentWindow(), SDL_WINDOW_FULLSCREEN);
        }
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F10)
    {
        isColorManual = !isColorManual;
    }
    if(pEvent.type == SDL_EVENT_WINDOW_RESIZED)
    {
        windowWidth = pEvent.window.data1;
        windowHeight = pEvent.window.data2;
        lScene.onResize(windowHeight, windowWidth);
        
        ge::gl::glViewport(0, 0, windowWidth, windowHeight);
    }
    #pragma region Camera Controls
    if(pEvent.type == SDL_EVENT_MOUSE_MOTION)
    {
        if(!isMouseRelative)
            return;
        float xoffset = static_cast<float>(pEvent.motion.xrel);
        float yoffset = static_cast<float>(pEvent.motion.yrel);
        lScene.lCamera.OnMouseMove(xoffset, yoffset);
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F1)
    {
        SDL_SetWindowRelativeMouseMode(SDL_GL_GetCurrentWindow(), !isMouseRelative); // Switch relative mouse mode
        isMouseRelative = !isMouseRelative;
    }
    if(pEvent.type == SDL_EVENT_MOUSE_WHEEL)
    {
        lScene.lCamera.OnMouseScroll(pEvent.wheel.y);
    }
    #pragma endregion
}
#pragma endregion

#pragma region Main Loop
int main()
{
    SDL_Window* lWindow = nullptr;
    SDL_GLContext context = nullptr;
    lScene.Initialise(&lWindow, &context, windowWidth, windowHeight, "Particle Sim");
    if(!lScene.isSceneInitialized())
    {
        std::cerr << "Scene initialization failed. Exiting..." << std::endl;
        return 1;
    }

    SDL_SetWindowRelativeMouseMode(lWindow, true);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui_ImplSDL3_InitForOpenGL(lWindow, context);
    ImGui_ImplOpenGL3_Init();

    if(lWindow == nullptr || context == nullptr)
    {
        lScene.Cleanup(lWindow, context);
        return 1;
    }

    lScene.InitCube();
    lScene.InitPlane();
    lScene.InitParticle(MAX_PARTICLE_NUMBER); // Initialize 100,000 particles
    bool running = true;
    auto previousFrame = SDL_GetPerformanceCounter();
    int activeParticleCount = 0; 
    float rColor = 0.0f;
    float gColor = 0.0f;
    float bColor = 1.0f;
    glm::vec3 particleColor(rColor, gColor, bColor);
    int maxParticleCount = MAX_PARTICLE_NUMBER; // Maximum number of particles
    
    #pragma region Main Loop
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            bool isMouseEvent = (event.type == SDL_EVENT_MOUSE_MOTION || 
                        event.type == SDL_EVENT_MOUSE_WHEEL || 
                        event.type == SDL_EVENT_MOUSE_BUTTON_DOWN || 
                        event.type == SDL_EVENT_MOUSE_BUTTON_UP);
            if(!isMouseRelative || !isMouseEvent)
            {
                ImGui_ImplSDL3_ProcessEvent(&event);
            }
            HandleEvent(event, running);
        }
        
        #pragma region Keyboard Input Handling
        const auto currentFrame = SDL_GetPerformanceCounter();
        const float deltaTime = static_cast<float>(currentFrame - previousFrame) /
            static_cast<float>(lFrequency);
        previousFrame = currentFrame;

        for(const auto& [key, pressed] : keys)
        {
            if(pressed)
                lScene.lCamera.OnKeyboardInput(key, deltaTime);
        }
        #pragma endregion

        // (After event loop)
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Once);
        ImGui::SetNextWindowSize(ImVec2(windowWidth*0.45f, windowHeight*0.25f), ImGuiCond_Once);
        ImGui::GetIO().FontGlobalScale = 1.5f;
        ImGui::Begin("Scene Parameters");
        ImGui::Text("Particles number : ");
        ImGui::SliderInt("##0", &activeParticleCount, 1, maxParticleCount);
        ImGui::Checkbox("Manual Color", &isColorManual);
        if(isColorManual)
        {
            // std::cout << "Manual color mode enabled. ("<< isColorManual << ")" << std::endl;
            ImGui::Text("R Color : ");
            ImGui::SliderFloat("##1", &rColor, 0.f, 1.f);
            ImGui::Text("B Color : ");
            ImGui::SliderFloat("##2", &bColor, 0.f, 1.f);
            ImGui::Text("G Color : ");
            ImGui::SliderFloat("##3", &gColor, 0.f, 1.f);
        }
        ImGui::Text("FPS: %.1f", FPS);
        ImGui::End();
        particleColor = glm::vec3(rColor, gColor, bColor);
        lScene.DrawScene(windowWidth, windowHeight, activeParticleCount, particleColor, isColorManual);
        
        #pragma region FPS Calculation
        if(FPSDebugMode)
        {
            frameCount++;
            FPS = frameCount/deltaTime;
            // std::cout << "FPS: " << FPS << std::endl; // Multiply by 2 because we measure over 1.0 seconds
            frameCount = 0;
            lTimer = currentFrame;
        
        }
        #pragma endregion
        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(lWindow);
    }
    #pragma endregion
    lScene.Cleanup(lWindow, context);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    return 0;
}
#pragma endregion
