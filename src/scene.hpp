#pragma once

#include <SDL3/SDL.h>
#include <geGL/geGL.h>
#include <string>
#include <memory>

class Scene
{
    public:
        /// @brief Creates an empty scene.
        Scene();

        /// @brief Creates the SDL window, OpenGL context, framebuffer, and shader programs.
        void Initialise(SDL_Window** pPWindow, SDL_GLContext* pPCtx,
                        int width, int height, const char* title);

        /// @brief Releases the scene graphics resources and destroys the SDL objects.
        void Cleanup(SDL_Window* pWindow, SDL_GLContext pCtx);

        /// @brief Creates the cube vertex and vertex-array objects.
        void InitCube();

        /// @brief Creates the plane vertex and vertex-array objects.
        void InitPlane();

        /// @brief Creates the requested number of particles.
        void InitParticle(int pNumber);

        /// @brief Draws the cube using its configured vertex array.
        void Draw3DCube();

        /// @brief Draws the plane using its configured vertex array.
        void Draw3DPlane();

        /// @brief Updates particles and renders the complete scene.
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
        /// @brief Loads a shader source file from the configured shader directory.
        std::string LoadShader(const std::string& fileName);

        /// @brief Reinitializes a particle after its lifetime expires.
        void ResetParticle(Particle& pParticle);
        std::vector<Particle> lParticles; // Vector to hold particles

};