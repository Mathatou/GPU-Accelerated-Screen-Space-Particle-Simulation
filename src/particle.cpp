#include "particle.hpp"

/// @brief Constructor for the Particle class. 
/// Initializes the particle's position, velocity, lifetime, size, and color. 
/// Put lifetime in the w coordinate of position and size in the w coordinate of velocity to save memory.
Particle::Particle(const glm::vec3& pPosition, 
    const glm::vec3& pVelocity, 
    float pLifetime, float pSize, const glm::vec4& pColor)
{
    position_lifetime = glm::vec4(pPosition, pLifetime);
    velocity_size = glm::vec4(pVelocity, pSize);
    color = pColor;
}

/// @brief Destructor for the Particle class.
Particle::~Particle()
{
}

/// @brief Moves the particle based on its velocity and decreases its lifetime by deltaTime. 
void Particle::Update(float deltaTime)
{
    position_lifetime.x += velocity_size.x * deltaTime;
    position_lifetime.y += velocity_size.y * deltaTime;
    position_lifetime.z += velocity_size.z * deltaTime;
    position_lifetime.w -= deltaTime;
}

/// @brief Returns true if the particle is still alive (lifetime > 0), false otherwise. 
bool Particle::IsAlive() const
{
    return position_lifetime.w > 0.0f;
}

/// @brief Get the position of the Particle 
const glm::vec3& Particle::GetPosition() const
{
    return *(const glm::vec3*)&position_lifetime;
}

/// @brief Get the velocity of the Particle 
const glm::vec3& Particle::GetVelocity() const
{
    return *(const glm::vec3*)&velocity_size;
}

/// @brief Get the lifetime of the Particle 
float Particle::GetLifetime() const
{
    return position_lifetime.w;
}

/// @brief Get the size of the particle.
float Particle::GetSize() const
{
    return velocity_size.w;
}

/// @brief Get the color of the particle. 
const glm::vec4& Particle::GetColor() const
{
    return color;
}