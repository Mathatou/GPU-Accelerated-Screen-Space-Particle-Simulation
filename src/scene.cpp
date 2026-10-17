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


using namespace ge::gl;
using namespace std;

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

    colorTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA8, 1, width, height);
    depthTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_DEPTH_COMPONENT32, 1, width, height);
    normalTexture = std::make_shared<Texture>(GL_TEXTURE_2D, GL_RGBA16F, 1, width, height);
    geometryBuffer = std::make_shared<Framebuffer>();
    geometryBuffer->attachTexture(GL_COLOR_ATTACHMENT0, colorTexture.get());
    geometryBuffer->attachTexture(GL_COLOR_ATTACHMENT1, normalTexture.get());
    geometryBuffer->attachTexture(GL_DEPTH_ATTACHMENT, depthTexture.get());

    geometryProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_geometry.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_geometry.frag")));
    displayProgram = std::make_shared<Program>(
        std::make_shared<Shader>(GL_VERTEX_SHADER, LoadShader("0_display.vert")),
        std::make_shared<Shader>(GL_FRAGMENT_SHADER, LoadShader("0_display.frag")));
    emptyVAO = std::make_shared<VertexArray>();
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
    const std::vector<float> vertices = {
         5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 0.0f, -5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 0.0f,  5.0f, -0.5f,  5.0f, 1.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 0.0f,  5.0f, -0.5f, -5.0f, 1.0f, 1.0f, 0.0f
    };
    planeVBO = std::make_shared<Buffer>(vertices.size() * sizeof(float), vertices.data());
    planeVAO = std::make_shared<VertexArray>();
    planeVAO->addAttrib(planeVBO, 0, 3, GL_FLOAT, 6 * sizeof(float), 0);
    planeVAO->addAttrib(planeVBO, 1, 3, GL_FLOAT, 6 * sizeof(float), 3 * sizeof(float));
}

void Scene::Draw3DCube()
{
    cubeVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    cubeVAO->unbind();
}

void Scene::Draw3DPlane()
{
    planeVAO->bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    planeVAO->unbind();
}

void Scene::DrawScene(int windowWidth, int windowHeight)
{
    glm::mat4 model = glm::rotate(glm::mat4(1.0f),
        static_cast<float>(SDL_GetTicks()) / 1000.0f * glm::radians(50.0f),
        glm::vec3(0.5f, 1.0f, 0.0f));
    const glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
        glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(windowWidth) / static_cast<float>(windowHeight), 0.1f, 100.0f);

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    displayProgram->use();
    normalTexture->bind(0);
    colorTexture->bind(1);
    depthTexture->bind(2);
    emptyVAO->bind();
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    emptyVAO->unbind();
}