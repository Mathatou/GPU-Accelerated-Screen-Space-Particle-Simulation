#version 450
in vec3 vNormal;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal;
uniform vec4 uColor;

void main()
{
    outColor = uColor;
    outNormal = vec4(vNormal, 1.0);
}