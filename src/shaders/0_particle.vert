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
    gl_PointSize = particles[gl_VertexID].velocity_size.w * 10.0;

    vParticleColor = particles[gl_VertexID].color;
}