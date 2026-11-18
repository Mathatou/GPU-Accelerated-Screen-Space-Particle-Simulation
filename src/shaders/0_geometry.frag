#version 450
in vec3 vNormal;
in vec4 worldPosition;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal_roughness;
uniform vec4 uColor;
uniform float uRoughness;
uniform vec3 uCameraPos;
uniform vec3 uLightDir;
uniform vec4 uLightColor;
uniform float uLightIntensity;

void main()
{
    float diffuse = max(dot(normalize(vNormal),-normalize(uLightDir)),
    0.0);
    float ambient = 0.25;
    float roughness = uRoughness;
    roughness = clamp(roughness, 0.0, 1.0);
    roughness = 1.0 - roughness; // Invert roughness for specular calculation
    vec3 viewDir = normalize(uCameraPos - worldPosition.xyz); // V 
    vec3 H = normalize(-normalize(uLightDir) + viewDir); // H
    float specular = pow(max(dot(normalize(vNormal), H), 0.0), roughness * 128.0);
    outNormal_roughness = vec4(normalize(vNormal), uRoughness);
    vec4 color = vec4(uColor.rgb * (((specular + diffuse) * uLightIntensity * uLightColor.rgb) + ambient), uColor.a);
    // vec4 correctedColor = vec4(pow(color.rgb, vec3(1.0/2.2)), color.a); // Apply gamma correction
    outColor = color;
    // outColor = vec4(vec3(specular),1.0);
}