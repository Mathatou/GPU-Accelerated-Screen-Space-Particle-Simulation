#version 450
in vec2 vTexCoord;
layout(binding = 0) uniform sampler2D uNormalTex;
layout(binding = 1) uniform sampler2D uColorTex;
layout(binding = 2) uniform sampler2D uDepthTex;

layout(location = 0)out vec4 outScreenColor;


void main()
{
    outScreenColor = texture(uNormalTex, vTexCoord)* vec4(0.5) + vec4(0.5);
}