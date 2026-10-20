#pragma region Includes
#include "scene.hpp"

#include <iostream>
#pragma endregion
#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define PARTICLE_NUMBER 100000

#pragma region Application State
int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;
auto lTimer = SDL_GetPerformanceCounter();
auto lFrequency = SDL_GetPerformanceFrequency();
int frameCount = 0;
float FPS = 0.f;
bool FPSDebugMode = true;
Scene lScene;
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
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F5)
    {
        std::cout << "Switching fps counter display mode." << std::endl;
        FPSDebugMode = !FPSDebugMode;
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_F11)
    {
        std::cout << "Switching fullscreen mode." << std::endl;
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
    #pragma region Camera Controls
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_Z)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_Z, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_S)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_S, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_D)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_D, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_Q)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_Q, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_SPACE)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_SPACE, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_LSHIFT)
    {
        lScene.lCamera.OnKeyboardInput(SDLK_LSHIFT, 0.016f); // Assuming a deltaTime of 16ms for this example
    }
    if(pEvent.type == SDL_EVENT_MOUSE_MOTION)
    {
        float xoffset = static_cast<float>(pEvent.motion.xrel);
        float yoffset = static_cast<float>(pEvent.motion.yrel);
        lScene.lCamera.OnMouseMove(xoffset, yoffset);
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
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    lScene.Initialise(&window, &context, windowWidth, windowHeight, "Particle Sim");
    if(!lScene.isSceneInitialized())
    {
        std::cerr << "Scene initialization failed. Exiting..." << std::endl;
        return 1;
    }

    if(window == nullptr || context == nullptr)
    {
        lScene.Cleanup(window, context);
        return 1;
    }

    lScene.InitCube();
    lScene.InitPlane();
    lScene.InitParticle(PARTICLE_NUMBER); // Initialize 100,000 particles
    bool running = true;
    while(running)
    {
        lScene.DrawScene(windowWidth, windowHeight);

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            HandleEvent(event, running);
        }
        SDL_GL_SwapWindow(window);
        
        #pragma region FPS Calculation
        if(FPSDebugMode)
        {
            frameCount++;
            auto lCurrentTime = SDL_GetPerformanceCounter();
            auto lElapsedTime = (lCurrentTime - lTimer) / static_cast<float>(lFrequency);
            if(lElapsedTime >=1.f)
            {
                FPS = frameCount/lElapsedTime;
                std::cout << "FPS: " << FPS << std::endl; // Multiply by 2 because we measure over 1.0 seconds
                frameCount = 0;
                lTimer = lCurrentTime;
            }
        }
        #pragma endregion
    }

    lScene.Cleanup(window, context);
    return 0;
}
#pragma endregion
