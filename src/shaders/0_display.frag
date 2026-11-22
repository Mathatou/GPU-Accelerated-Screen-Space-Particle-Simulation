#version 450
in vec2 vTexCoord;

layout(binding  = 0) uniform sampler2D uNormalTex;
layout(binding  = 1) uniform sampler2D uColorTex;
layout(binding  = 2) uniform sampler2D uDepthTex;

layout(location = 1) uniform mat4 uView;
layout(location = 2) uniform mat4 uProjection;
layout(location = 3) uniform vec3 uCameraPos;
layout(location = 4) uniform vec3 F0;
layout(location = 0)out vec4 outScreenColor;

vec3 FresnelSchlick(vec3 pNormal, vec3 pIncidence, vec3 F0)
{
    float cosTheta = clamp(dot(pNormal, pIncidence), 0.0, 1.0);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

bool IsOutsideScreen(vec2 pTexCoord)
{
    return pTexCoord.x < 0.0 || pTexCoord.x > 1.0 ||
           pTexCoord.y < 0.0 || pTexCoord.y > 1.0;
}

vec3 ReconstructWorldPosition(vec2 pTexCoord, float pDepth)
{
    float zNDC = pDepth * 2.0 - 1.0;
    vec4 clipPos = vec4(pTexCoord * 2.0 - 1.0, zNDC, 1.0);
    vec4 worldPos = inverse(uProjection * uView) * clipPos;
    return worldPos.xyz / worldPos.w;
}


vec3 ProjectToNDC(vec3 pWorldPos)
{
    vec4 clipPos = uProjection * uView * vec4(pWorldPos, 1.0);
    clipPos.xyz /= clipPos.w;
    return vec3(clipPos.xy * 0.5 + 0.5, clipPos.z);
}

vec2 RefineRayHit(vec3 pFrontPos, vec3 pBackPos)
{
    vec2 refinedUV = vec2(0.0);
    for(int j = 0; j < 5; ++j)
    {
        vec3 midPoint = (pFrontPos + pBackPos) * 0.5;
        vec3 midNDC = ProjectToNDC(midPoint);
        vec2 midTexCoord = midNDC.xy;
        refinedUV = midTexCoord;
        if(IsOutsideScreen(midTexCoord))
            break;
        float midSampledDepth = texture(uDepthTex, midTexCoord).r * 2.0 - 1.0;

        if(midSampledDepth < midNDC.z)
        {
            // Behind the surface, move the back position to the midpoint
            pBackPos = midPoint;
        }
        else
        {
            // In front of the surface, move the front position to the midpoint
            pFrontPos = midPoint;
        }
    }
    return refinedUV;
}


void TraceReflection(vec3 pWorldPos, vec3 pRayDir,
                     out vec4 pReflectionColor, out float pMixFactor)
{
    const float stepSize = 0.2;
    const float thickness = 0.08;
    const float maxSteps = 20/ stepSize;
    const vec2 screenCenter = vec2(0.5);

    pReflectionColor = vec4(0.0);
    pMixFactor = 0.0;

    float jitter = fract(sin(dot(gl_FragCoord.xy, vec2(12.9898, 78.233))) * 43758.5453);
    vec3 currentRayPos = pWorldPos + pRayDir * jitter * stepSize;
    vec3 prevRayPos = pWorldPos;
    for(int i = 0; i < int(maxSteps); ++i)
    {
        currentRayPos += pRayDir * stepSize;
        vec3 currentNDC = ProjectToNDC(currentRayPos);
        vec2 rayTexCoord = currentNDC.xy ;
        if(IsOutsideScreen(rayTexCoord))
            break;

        float sampledDepth = texture(uDepthTex, rayTexCoord).r * 2.0 - 1.0;
        
        if(sampledDepth < currentNDC.z &&
           sampledDepth > currentNDC.z - thickness)
        {
            vec2 hitUV = RefineRayHit(prevRayPos, currentRayPos);
            float edgeFade = smoothstep(0.5, 0.45, distance(hitUV, screenCenter));
            float distanceFade = 1.0 - float(i) / float(maxSteps);
            pReflectionColor = texture(uColorTex, hitUV);
            pMixFactor = 0.85 * edgeFade * distanceFade;
            break;
        }
        prevRayPos = currentRayPos;
    }
}

void main()
{
    vec4 baseColor = texture(uColorTex, vTexCoord);
    float depthValue = texture(uDepthTex, vTexCoord).r;
    if(depthValue >= 1.0)
    {
        outScreenColor = baseColor;
        return;
    }

    vec3 worldPos = ReconstructWorldPosition(vTexCoord, depthValue);
    vec4 normalData = texture(uNormalTex, vTexCoord);
    vec3 surfaceNormal = normalize(normalData.xyz);
    float roughness = normalData.a;

    vec3 incidence = normalize(worldPos - uCameraPos.xyz);
    vec3 viewDir = normalize(uCameraPos.xyz - worldPos);
    vec3 fresnel = FresnelSchlick(surfaceNormal, viewDir, F0);
    vec3 rayDir = reflect(incidence, surfaceNormal);

    if(roughness > 0.5)
    {
        outScreenColor = baseColor;
        return;
    }

    vec4 reflectionColor;
    float mixFactor;
    TraceReflection(worldPos, rayDir, reflectionColor, mixFactor);

    float reflectionFactor =
        fresnel.r * mixFactor * (1.0 - roughness);
    outScreenColor = mix(baseColor, reflectionColor, reflectionFactor);
}