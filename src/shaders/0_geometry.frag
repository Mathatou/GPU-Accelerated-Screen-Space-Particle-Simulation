#version 450
in vec3 vNormal;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal_roughness;
uniform vec4 uColor;
uniform float uRoughness;

void main()
{
    outColor = uColor;
    outNormal_roughness = vec4(normalize(vNormal), uRoughness);
}