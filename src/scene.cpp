#pragma region Includes
#include "scene.hpp"

#include <geGL/geGL.h>
#include <geGL/StaticCalls.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#pragma endregion

#pragma region Namespace Imports
using namespace ge::gl;
using namespace std;
#pragma endregion

#pragma region Scene Lifecycle
Scene::Scene()
{
}
std::string Scene::LoadShader(const std::string& fileName)
{
    const std::string filePath = shaderFolder + fileName;
    std::ifstream shaderFile(filePath);
    if(!shaderFile.is_open())
    {
        std::cerr << "Failed to open shader file: " << fileName
                << " - at path: " << filePath << std::endl;
        return "";
    }
    std::stringstream fileBuffer;
    fileBuffer << shaderFile.rdbuf();
    return fileBuffer.str();
}

void Scene::Initialise(SDL_Window** pPWindow, SDL_GLContext* pPCtx,
                int width, int height, const char* title)
{
    *pPWindow = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
    if(!*pPWindow)
    {
        std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
        return;
    }
    *pPCtx = SDL_GL_CreateContext(*pPWindow);
    if(!*pPCtx)
    {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        return;
    }
    
    ge::gl::init();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Create the framebuffer before attaching its color and depth textures.
    geometryBuffer = std::make_shared<Framebuffer>();
    initTextureAndAttach(geometryBuffer, colorTexture, GL_TEXTURE_2D, GL_RGBA8, width, height, GL_COLOR_ATTACHMENT0);
    initTextureAndAttach(geometryBuffer, normalTexture, GL_TEXTURE_2D, GL_RGBA16F, width, height, GL_COLOR_ATTACHMENT1);
    initTextureAndAttach(geometryBuffer, depthTexture, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, width, height, GL_DEPTH_ATTACHMENT);

    geometryProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_geometry.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_geometry.frag"))
    );
    displayProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_display.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_display.frag"))
    );
    computeProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_COMPUTE_SHADER, LoadShader("0_particle.comp"))
    );
    particleProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_particle.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_particle.frag"))
    );
    emptyVAO = std::make_shared<VertexArray>();
    bIsSceneInitialized = true;
    lCamera = Camera();
}

bool Scene::isSceneInitialized() const
{
    return bIsSceneInitialized;
}

void Scene::initTextureAndAttach(std::shared_ptr<Framebuffer>& framebuffer,
            std::shared_ptr<Texture>& texture,
            GLenum textureTarget,
            GLenum internalFormat,
            int width,
            int height,
            GLenum attachment)
{
    texture = std::make_shared<Texture>(textureTarget, internalFormat, 1, width, height);
    framebuffer->attachTexture(attachment, texture.get());
}


void Scene::onResize(int height, int width)
{
    // Update the scene's internal state when the window is resized
    initTextureAndAttach(geometryBuffer, colorTexture, GL_TEXTURE_2D, GL_RGBA8, width, height, GL_COLOR_ATTACHMENT0);
    initTextureAndAttach(geometryBuffer, normalTexture, GL_TEXTURE_2D, GL_RGBA16F, width, height, GL_COLOR_ATTACHMENT1);
    initTextureAndAttach(geometryBuffer, depthTexture, GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, width, height, GL_DEPTH_ATTACHMENT);
    lCamera.Update(); // Update camera matrices if necessary
}

void Scene::Cleanup(SDL_Window* pWindow, SDL_GLContext pCtx)
{
    geometryBuffer.reset();
    colorTexture.reset();
    normalTexture.reset();
    depthTexture.reset();
    SDL_GL_DestroyContext(pCtx);
    SDL_DestroyWindow(pWindow);
}

#pragma endregion
#pragma region Geometry

void Scene::InitCube()
{
    // 36 sommets : 3 floats pour la position, 3 floats pour la normale
    std::vector<float> vertices = {
        // Face Arrière (Z = -1)
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

    cubeVBO = std::make_shared<Buffer>(vertices.size() * sizeof(float), vertices.data());
    cubeVAO = std::make_shared<VertexArray>();
    cubeVAO->addAttrib(cubeVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    cubeVAO->addAttrib(cubeVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));
}

void Scene::InitPlane()
{
    // Pavé rectangulaire : largeur X = 20, profondeur Z = 10, épaisseur Y = 0.4.
    // Chaque sommet contient une position et une normale.
    const std::vector<float> vertices = {
        // Face supérieure
         10.0f, -0.5f,  5.0f, 0.0f,  1.0f, 0.0f,
        -10.0f, -0.5f,  5.0f, 0.0f,  1.0f, 0.0f,
        -10.0f, -0.5f, -5.0f, 0.0f,  1.0f, 0.0f,
         10.0f, -0.5f,  5.0f, 0.0f,  1.0f, 0.0f,
        -10.0f, -0.5f, -5.0f, 0.0f,  1.0f, 0.0f,
         10.0f, -0.5f, -5.0f, 0.0f,  1.0f, 0.0f,

        // Face inférieure
         10.0f, -0.9f, -5.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, -0.9f, -5.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, -0.9f,  5.0f, 0.0f, -1.0f, 0.0f,
         10.0f, -0.9f, -5.0f, 0.0f, -1.0f, 0.0f,
        -10.0f, -0.9f,  5.0f, 0.0f, -1.0f, 0.0f,
         10.0f, -0.9f,  5.0f, 0.0f, -1.0f, 0.0f,

        // Face avant (Z positif)
        -10.0f, -0.5f, 5.0f, 0.0f, 0.0f,  1.0f,
         10.0f, -0.5f, 5.0f, 0.0f, 0.0f,  1.0f,
         10.0f, -0.9f, 5.0f, 0.0f, 0.0f,  1.0f,
         10.0f, -0.9f, 5.0f, 0.0f, 0.0f,  1.0f,
        -10.0f, -0.9f, 5.0f, 0.0f, 0.0f,  1.0f,
        -10.0f, -0.5f, 5.0f, 0.0f, 0.0f,  1.0f,

        // Face arrière (Z négatif)
         10.0f, -0.5f, -5.0f, 0.0f, 0.0f, -1.0f,
        -10.0f, -0.9f, -5.0f, 0.0f, 0.0f, -1.0f,
         10.0f, -0.9f, -5.0f, 0.0f, 0.0f, -1.0f,
        -10.0f, -0.9f, -5.0f, 0.0f, 0.0f, -1.0f,
         10.0f, -0.5f, -5.0f, 0.0f, 0.0f, -1.0f,
        -10.0f, -0.5f, -5.0f, 0.0f, 0.0f, -1.0f,

        // Face gauche (X négatif)
        -10.0f, -0.5f, -5.0f, -1.0f, 0.0f, 0.0f,
        -10.0f, -0.9f,  5.0f, -1.0f, 0.0f, 0.0f,
        -10.0f, -0.5f,  5.0f, -1.0f, 0.0f, 0.0f,
        -10.0f, -0.5f, -5.0f, -1.0f, 0.0f, 0.0f,
        -10.0f, -0.9f, -5.0f, -1.0f, 0.0f, 0.0f,
        -10.0f, -0.9f,  5.0f, -1.0f, 0.0f, 0.0f,

        // Face droite (X positif)
         10.0f, -0.5f,  5.0f, 1.0f, 0.0f, 0.0f,
         10.0f, -0.9f,  5.0f, 1.0f, 0.0f, 0.0f,
         10.0f, -0.9f, -5.0f, 1.0f, 0.0f, 0.0f,
         10.0f, -0.9f, -5.0f, 1.0f, 0.0f, 0.0f,
         10.0f, -0.5f, -5.0f, 1.0f, 0.0f, 0.0f,
         10.0f, -0.5f,  5.0f, 1.0f, 0.0f, 0.0f,
    };

    
    planeVBO = std::make_shared<Buffer>(vertices.size() * sizeof(float), vertices.data());
    planeVAO = std::make_shared<VertexArray>();
    planeVAO->addAttrib(planeVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    planeVAO->addAttrib(planeVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));
}

#pragma endregion
#pragma region Particles

/// @brief Initializes a specified number of particles with random properties 
/// (position, velocity, lifetime, size, color) and adds them to the particle vector. 
void Scene::InitParticle(int pNumber)
{
    if(pNumber <= 0 || pNumber > 1000000) // Limit to 1 million particles for performance
    {
        std::cerr << "Invalid number of particles: " << pNumber << ". Must be between 1 and 1,000,000." << std::endl;
        return;
    }
    // Implementation for initializing particles
    for(int i = 0; i < pNumber; ++i)
    {
        glm::vec3 lPosition = glm::vec3(
            static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f,
            static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f,
            static_cast<float>(rand()) / RAND_MAX * 10.0f - 5.0f);
        glm::vec3 lVelocity = glm::vec3(
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f,
            static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f);
        float lLifetime = static_cast<float>(rand()) / RAND_MAX * 5.0f + 1.0f; // Lifetime between 1 and 6 seconds
        float lSize = static_cast<float>(rand()) / RAND_MAX * 0.5f + 0.1f; // Size between 0.1 and 0.6
        glm::vec4 lColor = glm::vec4(static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX,
            static_cast<float>(rand()) / RAND_MAX, 1.0f); // Random color with full alpha
        lParticles.push_back(Particle(lPosition, lVelocity, lLifetime, lSize, lColor));
    }
    InitParticleBuffer();
}


void Scene::InitParticleBuffer()
{
    if(lParticles.empty())
    {
        std::cerr << "No particles to initialize in the buffer." << std::endl;
        return;
    }

    // Create a buffer large enough to hold all particles
    // GL_DYNAMIC_DRAW : The data store contents will be modified repeatedly and used many times. 
    gParticleSSBO = std::make_shared<Buffer>(lParticles.size() * sizeof(Particle), lParticles.data(), GL_DYNAMIC_DRAW);
    gParticleSSBO->bindBase(GL_SHADER_STORAGE_BUFFER, 0); // Bind to binding point 0
}

#pragma endregion
#pragma region Rendering
void Scene::Draw3DCube()
{
    cubeVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeVAO->unbind();
}

void Scene::Draw3DPlane()
{
    planeVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    planeVAO->unbind();
}

void Scene::DrawParticles(glm::mat4 const& view, 
    glm::mat4 const& projection,
    glm::mat4 const& model
)
{
    particleProgram->use();
    // Set the view matrix for the particle shader
    // LookAt : Camera -- Center -- Up
    particleProgram->setMatrix4fv("uView",glm::value_ptr(view));
    particleProgram->setMatrix4fv("uProjection",glm::value_ptr(projection));
    particleProgram->setMatrix4fv("uModel",glm::value_ptr(model));
    glDisable(GL_DEPTH_TEST);
    emptyVAO->bind();
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(lParticles.size()));
    emptyVAO->unbind();
    glEnable(GL_DEPTH_TEST);
}


/// @brief Draw the 3D scene, including the cube, plane, and particles. 
/// Updates particle states and resets them if they are no longer alive. 
void Scene::DrawScene(int windowWidth, int windowHeight)
{
    glm::mat4 model = glm::rotate(glm::mat4(1.0f),
        static_cast<float>(SDL_GetTicks()) / 1000.0f * glm::radians(50.0f),
        glm::vec3(0.5f, 1.0f, 0.0f));
    // const glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
    //     glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 view = glm::mat4(1.0f);
    lCamera.Update();
    view = lCamera.GetViewMatrix(view);
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

    // ====== PARTICLE COMPUTE PASS ======
    gParticleSSBO->bindBase(GL_SHADER_STORAGE_BUFFER, 0);
    computeProgram->use();
    // The compute shader uses this value to advance position and lifetime.
    glUniform1f(glGetUniformLocation(computeProgram->getId(), "uDeltaTime"), 0.016f);
    const float deltaTime = 0.016f; // Assuming a fixed time step of 16ms (60 FPS)
    computeProgram->set1f("uDeltaTime",deltaTime);
    computeProgram->setMatrix4fv("uView",glm::value_ptr(view));
    computeProgram->setMatrix4fv("uProjection",glm::value_ptr(projection));
    normalTexture->bind(0);
    depthTexture->bind(2);
    
    // Compute the number of work groups needed based on the number of particles
    const GLuint workGroupCount = static_cast<GLuint>((lParticles.size() + 255) / 256);
    computeProgram->dispatch(workGroupCount, 1, 1);
    // Ensure that the compute shader has finished writing to the SSBO before rendering
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // ====== GEOMETRY PASS ======
    geometryBuffer->bind();
    geometryProgram->use();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    geometryBuffer->drawBuffers({GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1});
    geometryProgram->setMatrix4fv("uModel", glm::value_ptr(model));
    geometryProgram->setMatrix4fv("uView", glm::value_ptr(view));
    geometryProgram->setMatrix4fv("uProjection", glm::value_ptr(projection));
    Draw3DCube();

    model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.7f, 0.7f));
    geometryProgram->setMatrix4fv("uModel", glm::value_ptr(model));
    Draw3DPlane();
    geometryBuffer->unbind();

    // ====== DISPLAY PASS ======
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    displayProgram->use();
    normalTexture->bind(0);
    colorTexture->bind(1);
    depthTexture->bind(2);
    emptyVAO->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    emptyVAO->unbind();
    
    model = glm::mat4(1.0f);
    
    DrawParticles(view, projection, model);


    #pragma endregion
    
}