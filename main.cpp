#include <SDL3/SDL.h>

#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>
#include <iostream>
int WIN_W = 1024;
int WIN_H = 768;
SDL_Window* window;
SDL_GLContext ctx;

using namespace ge::gl;

/// @brief Initialises the SDL window and OpenGL context
/// @param pPWindow 
/// @param pPCtx 
/// @param width 
/// @param height 
/// @param title 
void Initialise(SDL_Window **pPWindow, 
                SDL_GLContext *pPCtx, 
                int width = 1024, 
                int height = 768, 
                const char* title = "Particle Sim")
{
    *pPWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
    if(!pPWindow)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
    }
    *pPCtx = SDL_GL_CreateContext(*pPWindow);
    if(!pPCtx)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
    }
    
}

/// @brief Cleans up the SDL window and OpenGL context 
void Cleanup(SDL_Window *pWindow, SDL_GLContext pCtx)
{
    SDL_GL_DestroyContext(pCtx);
    SDL_DestroyWindow(pWindow);
}

/// @brief Handles SDL events such as quitting the application and mouse movement
void EventHandling(SDL_Event pEvent, bool &pRunning)
{
    // Quit using button
    if(pEvent.type == SDL_EVENT_QUIT)
    {
        pRunning = false;
    }
    //Quit using esc
    if(pEvent.type == SDL_EVENT_KEY_DOWN)
    {
        if(pEvent.key.key == SDLK_ESCAPE)
        {
            pRunning = false; std::cout << "Escape key pressed. Exiting..." << std::endl;
        }
    }
    if(pEvent.type == SDL_EVENT_MOUSE_MOTION)
    {
        // std::cout << "Mouse moved to (" << pEvent.motion.x << ", " << pEvent.motion.y << ")" << std::endl;
        glClearColor(pEvent.motion.x / 1024.0f, pEvent.motion.y / 768.0f, 0.0f, 1.0f); // sets clear color based on mouse position
    }
}

int main()
{
    ge::gl::init();
    Initialise(&window, &ctx, WIN_W, WIN_H, "Particle Sim");
    bool running = true;
    while(running) // main loop
    {
        SDL_Event event;
        // Event loop
        while(SDL_PollEvent(&event))
        {
            EventHandling(event, running);
        }
        
        glClear(GL_COLOR_BUFFER_BIT); // clear color buffer
        SDL_GL_SwapWindow(window); // swap front & back buffers
    }

    Cleanup(window, ctx);
    return 0;
}