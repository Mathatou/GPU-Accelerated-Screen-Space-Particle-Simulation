#pragma once
#include <glm/glm.hpp>


class Particle
{
    
    public:
        Particle(const glm::vec3& pPosition, 
                const glm::vec3& pVelocity, 
                float pLifetime, 
                float pSize, 
                const glm::vec4& pColor);
        ~Particle();
        
        void Update(float deltaTime);
        bool IsAlive() const;
        
        const glm::vec3& GetPosition() const;
        const glm::vec3& GetVelocity() const;
        float GetLifetime() const;
        float GetSize() const;
        const glm::vec4& GetColor() const;
            
    private:
        Particle();
        // On met lifetime dans la coordonnée w de la position pour économiser de la mémoire
        glm::vec4 position_lifetime;
        // float lifetime;
        // On met size dans la coordonnée w de la position pour économiser de la mémoire
        glm::vec4 velocity_size;
        // float size;
        glm::vec4 color;

        void InitParticle();
};
