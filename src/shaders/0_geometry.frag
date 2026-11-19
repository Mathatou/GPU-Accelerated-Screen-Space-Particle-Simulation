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

float CalculateShadowPCF(vec3 pPosLightSpace, vec3 pNormal, vec3 pLightDir)
{
    float shadow = 0.0;
    vec3 projectedPosition = pPosLightSpace * 0.5 + 0.5;

    if(projectedPosition.z < 0.0)
        return 0.0;

    if(projectedPosition.z > 1.0)
        return 0.0;

    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    float bias = max(
        0.05 * (1.0 - dot(pNormal, pLightDir)),
        0.005
    );
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            float closestDepth = texture(uShadowMap, vec3(projectedPosition.xy + offset, projectedPosition.z-bias));
            shadow += 1-closestDepth;
        }
    }

    return clamp(shadow / 9.0, 0.0, 1.0);
}

// float CalculateShadow(vec3 pPosLightSpace, vec3 pNormal, vec3 pLightDir)
// {
//     vec3 projectedPosition = pPosLightSpace * 0.5 + 0.5;

//     if(projectedPosition.z < 0.0)
//         return 0.0;
//     if(projectedPosition.z > 1.0)
//         return 0.0;

//     float closestDepth = texture(uShadowMap, projectedPosition.xy).r;
//     float bias = max(
//         0.05 * (1.0 - dot(pNormal, pLightDir)),
//         0.005
//     );

//     return projectedPosition.z - bias > closestDepth ? 1.0 : 0.0;
// }

void main()
{
    vec3 normal = normalize(vNormal);
    vec3 lightDir = -normalize(uLightDir);

    float shadow = CalculateShadowPCF(
        vertPosLightSpace.xyz,
        normal,
        lightDir
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