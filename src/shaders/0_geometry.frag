#version 450
in vec3 vNormal;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal;


void main()
{
    outColor = vec4(1.0, 0.0, 0.0, 1.0);
    outNormal = vec4(vNormal, 1.0);
}