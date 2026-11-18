#version 450
in vec3 vNormal;
in vec4 worldPosition;
in vec4 vertPosLightSpace;

layout(location = 0)out vec4 outColor;
layout(location = 1)out vec4 outNormal_roughness;
uniform vec4 uColor;
uniform float uRoughness;
uniform vec3 uCameraPos;
uniform vec3 uLightDir;
uniform vec4 uLightColor;
uniform float uLightIntensity;
layout(binding = 3) uniform sampler2D uShadowMap;

float CalculateShadow(vec3 posLightSpace, vec3 normal, vec3 lightDir)
{
    // Remap NDC [-1, 1] to texture coordinates [0, 1]
    vec3 projCoords = posLightSpace * 0.5 + 0.5;

    // Prevent shadows beyond the light's far plane
    if (projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005); // Bias to prevent shadow acne
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}
void main()
{
    float shadow = CalculateShadow(vertPosLightSpace.xyz, 
    normalize(vNormal), 
    -normalize(uLightDir));
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
    vec4 color = vec4(uColor.rgb * (((1.0 - shadow) * (specular + diffuse) * uLightIntensity * uLightColor.rgb) + ambient), uColor.a);
    // vec4 correctedColor = vec4(pow(color.rgb, vec3(1.0/2.2)), color.a); // Apply gamma correction
    outColor = color;
}