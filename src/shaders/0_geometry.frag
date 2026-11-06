#version 450
in vec3 vNormal;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal_roughness;
uniform vec4 uColor;
uniform float uRoughness;
uniform vec3 uLightDir;
uniform vec4 uLightColor;
uniform float uLightIntensity;

void main()
{
    float diffuse = max(dot(normalize(vNormal),-normalize(uLightDir)),
    0.0);
    float ambient = 0.1;
    outColor = vec4(uColor.rgb * (diffuse * uLightIntensity * uLightColor.rgb + ambient), uColor.a);
    outNormal_roughness = vec4(normalize(vNormal), uRoughness);
}