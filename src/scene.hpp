#pragma once

#include <SDL3/SDL.h>
#include <geGL/geGL.h>
#include <string>
#include <memory>

class Scene
{
    public:
        Scene();
        void Initialise(SDL_Window** pPWindow, SDL_GLContext* pPCtx,
                        int width, int height, const char* title);
        void Cleanup(SDL_Window* pWindow, SDL_GLContext pCtx);
        void InitCube();
        void InitPlane();
        void Draw3DCube();
        void Draw3DPlane();
        void DrawScene(int windowWidth, int windowHeight);
        
    private:
        const std::string shaderFolder = "../src/shaders/";
        std::shared_ptr<ge::gl::Framebuffer> geometryBuffer;
        std::shared_ptr<ge::gl::Texture> normalTexture;
        std::shared_ptr<ge::gl::Texture> depthTexture;
        std::shared_ptr<ge::gl::Texture> colorTexture;
        std::shared_ptr<ge::gl::Program> geometryProgram;
        std::shared_ptr<ge::gl::Program> displayProgram;
        std::shared_ptr<ge::gl::Buffer> cubeVBO;
        std::shared_ptr<ge::gl::Buffer> planeVBO;
        std::shared_ptr<ge::gl::VertexArray> cubeVAO;
        std::shared_ptr<ge::gl::VertexArray> planeVAO;
        std::shared_ptr<ge::gl::VertexArray> emptyVAO;
        std::string LoadShader(const std::string& fileName);

};