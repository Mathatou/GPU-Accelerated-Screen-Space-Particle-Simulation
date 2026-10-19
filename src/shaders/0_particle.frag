#version 450

layout(location = 0) out vec4 vOutputColor;
layout(location = 0) in vec4 vParticleColor;

void main()
{
    vOutputColor = vParticleColor;
}