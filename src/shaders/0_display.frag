#version 450
in vec2 vTexCoord;

layout(binding  = 0) uniform sampler2D uNormalTex;
layout(binding  = 1) uniform sampler2D uColorTex;
layout(binding  = 2) uniform sampler2D uDepthTex;

layout(location = 1) uniform mat4 uView;
layout(location = 2) uniform mat4 uProjection;
layout(location = 3) uniform vec3 uCameraPos;

layout(location = 0)out vec4 outScreenColor;


void main()
{
    vec4 baseColor = texture(uColorTex, vTexCoord);
    float depthValue = texture(uDepthTex, vTexCoord).r;
    if(depthValue >= 1.0) // Assuming 1.0 indicates no geometry
    {
        outScreenColor = baseColor; // Output the base color directly
        return;
    }

    // Convert depth from [0, 1] to [-1, 1]
    float zNDC = depthValue * 2.0 - 1.0; // Assuming depth is stored in the red channel
    vec4 clipSpacePos = vec4(vTexCoord*2.0 - 1.0, zNDC,1.0f); 
    
    vec4 worldPos4D = inverse(uProjection*uView)*clipSpacePos; 
    vec3 worldPos = worldPos4D.xyz/ worldPos4D.w; // Perspective divide to get world position

    vec3 incidence = normalize(worldPos - uCameraPos.xyz);
    vec3 surfaceNormal = normalize(texture(uNormalTex,vTexCoord).xyz);
    vec3 rayDir = reflect(incidence, surfaceNormal);
    
    float stepSize = 0.1; // Adjust step size as needed
    float thickness = 0.05;
    vec4 reflectionColor = vec4(0.0); 
    float mixFactor = 0;

    vec3 currentRayPos = worldPos; // Start from the current world position
    for(int i = 0; i < 100; ++i)
    {
        // Move the ray forward in the reflected direction
        currentRayPos += rayDir * stepSize; // Adjust step size as needed

        // Project the new world position back to clip space
        vec4 newClipSpacePos = uProjection * uView * vec4(currentRayPos, 1.0);
        newClipSpacePos.xyz /= newClipSpacePos.w; // Perspective divide

        // Convert to texture coordinates [0,1]
        vec2 newTexCoord = newClipSpacePos.xy * 0.5 + 0.5;

        // Check if the new texture coordinates are within bounds
        if(newTexCoord.x < 0.0 || newTexCoord.x > 1.0 || newTexCoord.y < 0.0 || newTexCoord.y > 1.0)
            break; // Exit if out of bounds

        // Sample the depth at the new texture coordinates
        // Convert to [-1, 1]
        float sampledDepth = texture(uDepthTex, newTexCoord).r *2.0 - 1.0; 

        // If the new depth is less than the current depth, we hit a surface
        if(sampledDepth < newClipSpacePos.z && sampledDepth > newClipSpacePos.z - thickness)
        {
            reflectionColor = texture(uColorTex, newTexCoord);
            mixFactor = 0.35; // Adjust the mix factor as needed
            break;
        }
    }
    // If no surface was hit, output a default color (e.g., black)
    outScreenColor = mix(baseColor, reflectionColor, mixFactor); // Blend the reflection with the base color
}