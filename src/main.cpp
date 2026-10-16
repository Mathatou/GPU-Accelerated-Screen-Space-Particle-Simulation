#pragma region Includes
    #include <SDL3/SDL.h>

    #include <geGL/geGL.h>
    #include <geGL/StaticCalls.h>

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>

    #include <fstream>
    #include <sstream>
    #include <string>
    #include <iostream>
#pragma endregion

#pragma region Global Variables
    int WIN_W = 1024;
    int WIN_H = 768;
    SDL_Window* window;
    SDL_GLContext ctx;

    using namespace ge::gl;
    std::string lShaderFolder = "../src/shaders/";
    decltype(std::make_shared<Framebuffer>()) gGeometryBuffer;
    decltype(std::make_shared<Texture>()) gNormalTexture;
    decltype(std::make_shared<Texture>()) gDepthTexture;
    decltype(std::make_shared<Texture>()) gColorTexture;
    decltype(std::make_shared<Program>()) lGeometryProgram;
    decltype(std::make_shared<Program>()) lDisplayProgram;
    decltype(std::make_shared<Buffer>()) lCubeVBO;
    decltype(std::make_shared<Buffer>()) lPlaneVBO;
    decltype(std::make_shared<VertexArray>()) lCubeVAO;
    decltype(std::make_shared<VertexArray>()) lPlaneVAO;
    decltype(std::make_shared<VertexArray>()) lEmptyVAO;
#pragma endregion

/// @brief Loads a shader file and returns its contents as a string
/// @param fileName Name of the shader file to load
/// @return The shader file contents as a string, or an empty string if the file could not be opened
std::string LoadShader(const std::string fileName)
{
    std::string lFilePath = lShaderFolder + fileName;
    std::ifstream shaderFile(lFilePath);

    if(!shaderFile.is_open())
    {
        std::cerr << "Failed to open shader file: " << fileName <<"- at path: " << lFilePath << std::endl;
        return "";
    }
    std::stringstream lNameFileBuffer;
    lNameFileBuffer << shaderFile.rdbuf();
    return lNameFileBuffer.str();
}

/// @brief Initialises the SDL window and OpenGL context
void Initialise(SDL_Window **pPWindow, 
                SDL_GLContext *pPCtx, 
                int width = 1024, 
                int height = 768, 
                const char* title = "Particle Sim")
{
    #pragma region SDL_Init & geGL
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
    ge::gl::init();

    #pragma endregion
    
    #pragma region Buffer & Texture Setup
    gColorTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA8, 1, width, height);
    gDepthTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, 1 , width, height);
    gNormalTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA16F, 1, width, height);
    gGeometryBuffer = std::make_shared<Framebuffer>();
    gGeometryBuffer->attachTexture(GL_COLOR_ATTACHMENT0, gColorTexture.get());
    gGeometryBuffer->attachTexture(GL_COLOR_ATTACHMENT1, gNormalTexture.get());
    gGeometryBuffer->attachTexture(GL_DEPTH_ATTACHMENT, gDepthTexture.get());
    #pragma endregion

    #pragma region Shader Setup
    lGeometryProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_geometry.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_geometry.frag"))
    );
    lDisplayProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_display.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_display.frag"))
    );
    lEmptyVAO = std::make_shared<VertexArray>();
    #pragma endregion
    
    
}

/// @brief Cleans up the SDL window and OpenGL context 
void Cleanup(SDL_Window *pWindow, SDL_GLContext pCtx)
{
    gGeometryBuffer.reset();
    gColorTexture.reset();
    gNormalTexture.reset();
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
        // glClearColor(pEvent.motion.x / 1024.0f, pEvent.motion.y / 768.0f, 0.0f, 1.0f); // sets clear color based on mouse position
    }
}


void InitCube()
{
    std::vector<float> cubeVertices = 
    {
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

        // Face Avant (Z = 1)
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

        // Face Gauche (X = -1)
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

        // Face Droite (X = 1)
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

        // Face Bas (Y = -1)
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
        -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,

        // Face Haut (Y = 1)
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
    };
    lCubeVBO = std::make_shared<Buffer>(cubeVertices.size() * sizeof(float), cubeVertices.data());
    lCubeVAO = std::make_shared<VertexArray>();

    lCubeVAO->addAttrib(lCubeVBO,0,3,GL_FLOAT,6*sizeof(float),0);
    lCubeVAO->addAttrib(lCubeVBO,1,3,GL_FLOAT,6*sizeof(float),3*sizeof(float));
}

/// @brief Draws a 3D cube using the current OpenGL context
void Draw3DCube()
{
    lCubeVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    lCubeVAO->unbind();
    
}
void InitPlane()
{
    std::vector<float> planeVertices = 
    {
        // Positions          // Normals
         5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 0.0f,

         5.0f, -0.5f,  5.0f,  1.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 0.0f,
         5.0f, -0.5f, -5.0f,  1.0f, 1.0f, 0.0f
    };
    lPlaneVBO = std::make_shared<Buffer>(planeVertices.size() * sizeof(float), planeVertices.data());
    lPlaneVAO = std::make_shared<VertexArray>();

    lPlaneVAO->addAttrib(lPlaneVBO, 0,3 ,GL_FLOAT ,6*sizeof(float), 0);
    lPlaneVAO->addAttrib(lPlaneVBO, 1 ,3 ,GL_FLOAT ,6*sizeof(float), 3*sizeof(float));
}
void Draw3DPlane()
{
    lPlaneVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    lPlaneVAO->unbind();
}

/// @brief Draws the scene by rendering to the geometry buffer and then displaying it
void DrawScene()
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, 
        (float)SDL_GetTicks() / 1000.0f * glm::radians(50.0f), 
        glm::vec3(0.5f, 1.0f, 0.0f));
    // Set up the view matrix
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f), // Camera Position
        glm::vec3(0.0f, 0.0f, 0.0f), // Target Position
        glm::vec3(0.0f, 1.0f, 0.0f) // Up vector
    );
    // Setup the projection matrix
    glm::mat4 projection = glm::perspective(
        glm::radians(45.0f), 
        (float)WIN_W / (float)WIN_H, 
        0.1f, 
        100.0f);

    // =========== G-BUFFER PASS ===========
    gGeometryBuffer->bind();
    lGeometryProgram->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer & depth buffer

    gGeometryBuffer->drawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});
    lGeometryProgram->setMatrix4fv("uModel", glm::value_ptr(model));
    lGeometryProgram->setMatrix4fv("uView", glm::value_ptr(view));
    lGeometryProgram->setMatrix4fv("uProjection", glm::value_ptr(projection));
    Draw3DCube();
    
    // Dessin du plan plus bas que le cube pour qu'il soit visible
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.7f, 0.7f));
    lGeometryProgram->setMatrix4fv("uModel", glm::value_ptr(model));
    Draw3DPlane();

    gGeometryBuffer->unbind();

    // =========== DISPLAY PASS ===========
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer & depth buffer
    
    lDisplayProgram->use();
    gNormalTexture->bind(0); // binding is done in the shader using layout(binding = 0)
    gColorTexture->bind(1); // binding is done in the shader using layout(binding = 1)
    gDepthTexture->bind(2); // binding is done in the shader using layout(binding = 2)
    lEmptyVAO->bind();
    // lDisplayProgram->set1i("uNormalTex", 0);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    lEmptyVAO->unbind();
}

int main()
{
    Initialise(&window, &ctx, WIN_W, WIN_H, "Particle Sim");
    InitCube();
    InitPlane();
    bool running = true;
    /// LMAIN LOOP
    #pragma region MainLoop
    glEnable(GL_DEPTH_TEST);
    while(running) 
    {
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // set clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer
        
        DrawScene();
        
        #pragma region EventHandling
        SDL_Event event;
        // Event loop
        while(SDL_PollEvent(&event))
        {
            EventHandling(event, running);
        }
#pragma endregion
        
        SDL_GL_SwapWindow(window); // swap front & back buffers
    }
    #pragma endregion
    Cleanup(window, ctx);
    return 0;
}