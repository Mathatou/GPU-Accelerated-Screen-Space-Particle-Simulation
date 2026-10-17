#include "scene.hpp"

#include <iostream>

int windowWidth = 1024;
int windowHeight = 768;
Scene lScene;

void HandleEvent(const SDL_Event& pEvent, bool& running)
{
    if(pEvent.type == SDL_EVENT_QUIT)
    {
        running = false;
    }
    else if(pEvent.type == SDL_EVENT_KEY_DOWN && pEvent.key.key == SDLK_ESCAPE)
    {
        running = false;
        std::cout << "Escape key pressed. Exiting..." << std::endl;
    }
}


int main()
{
    SDL_Window* window = nullptr;
    SDL_GLContext context = nullptr;
    
    lScene.Initialise(&window, &context, windowWidth, windowHeight, "Particle Sim");
    if(window == nullptr || context == nullptr)
    {
        lScene.Cleanup(window, context);
        return 1;
    }

    lScene.InitCube();
    lScene.InitPlane();

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
    }

    lScene.Cleanup(window, context);
    return 0;
}
