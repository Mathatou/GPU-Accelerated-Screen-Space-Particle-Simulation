#version 450
in layout(location = 0) vec3 inPosition;
in layout(location = 1) vec3 inNormal;
uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform bool bIsWater; 
uniform float iTime; 
out vec3 vNormal;
out vec4 worldPosition;
out vec4 vertPosLightSpace;

void ApplyWaterDisplacement(inout vec4 pWorldPosition)
{
    const float wave1Amplitude = 0.03;
    const float wave2Amplitude = 0.02;

    float wave1 = 0.07 + wave1Amplitude *
        sin(iTime * 2.0 + pWorldPosition.x * 1.5);
    float wave2 = 0.03 + wave2Amplitude *
        sin(iTime * 3.5 + pWorldPosition.z * 3);

    pWorldPosition.y += wave1 + wave2;

    float derivativeX =
        wave1Amplitude * 1.5 *
        cos(iTime * 2.0 + pWorldPosition.x * 1.5);
    float derivativeZ =
        wave2Amplitude * 3.0 *
        cos(iTime * 3.5 + pWorldPosition.z * 3);

    vNormal = normalize(vec3(-derivativeX, 1.0, -derivativeZ));
}

void main()
{
    worldPosition = uModel * vec4(inPosition, 1.0);

    if(bIsWater)
    {
        ApplyWaterDisplacement(worldPosition);
    }
    else
    {
        mat3 normalMatrix = transpose(inverse(mat3(uModel)));
        vNormal = normalize(normalMatrix * inNormal);
    }

    vertPosLightSpace = vec4(vec3(uLightSpaceMatrix * worldPosition), 1.0);
    gl_Position = uProjection * uView * worldPosition;
}