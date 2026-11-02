#version 450

layout(location = 0) out vec4 vParticleColor;

struct Particle
{
    vec4 position_lifetime;
    vec4 velocity_size;
    vec4 color; 
};

layout(std430, binding = 0) buffer ParticleBufferData
{
    Particle particles[];
};

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main()
{
    vec4 worldPosition = uModel * vec4(
        particles[gl_VertexID].position_lifetime.xyz, 
        1.0
        );
    // Position of the particle in clip space
    gl_Position = uProjection * uView * worldPosition;
    float speed = length(particles[gl_VertexID].velocity_size.xyz) ; // Scale size based on velocity magnitude
    gl_PointSize = (speed / gl_Position.w)*2.0 ; // Adjust the multiplier as needed for visual effect
    gl_PointSize = clamp(gl_PointSize, 3.0, 6.0); // Clamp the point size to avoid too small or too large points

    vParticleColor = particles[gl_VertexID].color;
}