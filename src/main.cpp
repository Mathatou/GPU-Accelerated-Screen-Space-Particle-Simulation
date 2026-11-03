#pragma region Includes
#include "scene.hpp"

#include <iostream>
#include <map>
#pragma endregion
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PARTICLE_NUMBER 10000

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
    SDL_SetWindowRelativeMouseMode(lWindow, true); // Enable relative mouse mode for capturing mouse movement
    if(!lScene.isSceneInitialized())
    {
        std::cerr << "Scene initialization failed. Exiting..." << std::endl;
        return 1;
    }

    if(lWindow == nullptr || context == nullptr)
    {
        lScene.Cleanup(lWindow, context);
        return 1;
    }

    lScene.InitCube();
    lScene.InitPlane();
    lScene.InitParticle(PARTICLE_NUMBER); // Initialize 100,000 particles
    bool running = true;
    auto previousFrame = SDL_GetPerformanceCounter();
    while(running)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            HandleEvent(event, running);
        }
        
        const auto currentFrame = SDL_GetPerformanceCounter();
        const float deltaTime = static_cast<float>(currentFrame - previousFrame) /
            static_cast<float>(lFrequency);
        previousFrame = currentFrame;

        for(const auto& [key, pressed] : keys)
        {
            if(pressed)
                lScene.lCamera.OnKeyboardInput(key, deltaTime);
        }
        
        lScene.DrawScene(windowWidth, windowHeight);
        SDL_GL_SwapWindow(lWindow);
        
        #pragma region FPS Calculation
        if(FPSDebugMode)
        {
            frameCount++;
            FPS = frameCount/deltaTime;
            std::cout << "FPS: " << FPS << std::endl; // Multiply by 2 because we measure over 1.0 seconds
            frameCount = 0;
            lTimer = currentFrame;
        
        }
        #pragma endregion
    }

    lScene.Cleanup(lWindow, context);
    return 0;
}
#pragma endregion
