#version 450
in vec3 vNormal;
in vec4 worldPosition;
in vec4 vertPosLightSpace;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 outNormalRoughness;

uniform vec4 uColor;
uniform float uRoughness;
uniform vec3 uCameraPos;
uniform vec3 uLightDir;
uniform vec4 uLightColor;
uniform float uLightIntensity;

layout(binding = 3) uniform sampler2DShadow uShadowMap;

float CalculateShadowPCF(vec3 pPosLightSpace, vec3 pNormal, vec3 pLightDir, float sampleNumber)
{
    float shadow = 0.0;
    vec3 projectedPosition = pPosLightSpace * 0.5 + 0.5;
    float visibility = 0.0;

    if(projectedPosition.z < 0.0)
        return 0.0;

    if(projectedPosition.z > 1.0)
        return 0.0;

    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float bias = max(
        0.05 * (1.0 - dot(pNormal, pLightDir)),
        0.005
    );
    // Poisson disk sampling for percentage-closer filtering (PCF)
    const vec2 poissonDisk[16] = vec2[](
    vec2(-0.94201624, -0.39906216), vec2( 0.94558609, -0.76890725),
    vec2(-0.09418410, -0.92938870), vec2( 0.34495938,  0.29387760),
    vec2(-0.91588581,  0.45771432), vec2(-0.81544232, -0.87912464),
    vec2(-0.38277543,  0.27676845), vec2( 0.97484398,  0.75648379),
    vec2( 0.44323325, -0.97511554), vec2( 0.53742981, -0.47373420),
    vec2(-0.26496911, -0.41893023), vec2( 0.79197514,  0.19090188),
    vec2(-0.24188840,  0.99706507), vec2(-0.81409955,  0.91437590),
    vec2( 0.19984126,  0.78641367), vec2( 0.14383161, -0.14100790)
    );

    float angle = fract(sin(dot(worldPosition.xz, vec2(12.9898, 78.233))) * 43758.5453) * 6.283185;
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    float spread = 2.5;
    
    for (int i=0;i<sampleNumber;i++)
    {
        vec2 offset = rot * poissonDisk[i] * texelSize * spread;
        visibility = texture(uShadowMap, vec3(projectedPosition.xy + offset, projectedPosition.z-bias))   ;
        shadow+=1-visibility;
    }

    return clamp(shadow / sampleNumber, 0.0, 1.0);
}

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = -normalize(uLightDir);

    float shadow = CalculateShadowPCF(
        vertPosLightSpace.xyz,
        normal,
        lightDir,
        16.0
    );
    float diffuse = max(dot(normal, lightDir), 0.0);

    float specularRoughness = 1.0 - clamp(uRoughness, 0.0, 1.0);
    vec3 viewDir = normalize(uCameraPos - worldPosition.xyz);
    vec3 halfVector = normalize(lightDir + viewDir);
    float specular = pow(
        max(dot(normal, halfVector), 0.0),
        specularRoughness * 128.0
    );

    const float ambient = 0.25;
    vec3 litColor = uColor.rgb *
        (((1.0 - shadow) * (specular + diffuse) *
          uLightIntensity * uLightColor.rgb) + ambient);

    outNormalRoughness = vec4(normal, uRoughness);
    outColor = vec4(litColor, uColor.a);
}