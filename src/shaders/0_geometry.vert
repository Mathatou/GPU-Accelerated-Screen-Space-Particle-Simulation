#version 450
in layout(location = 0) vec3 inPosition;
in layout(location = 1) vec3 inNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform bool bIsWater; 
uniform float iTime; 
out vec3 vNormal;


void main()
{
    vec4 worldPosition = uModel * vec4(inPosition, 1.0);
    if(bIsWater)
    {
        const float wave1Amplitude = 0.03;
        const float wave2Amplitude = 0.02;
        float wave1 = 0.07 + wave1Amplitude * sin(iTime * 2.0 + worldPosition.x * 1.5);
        float wave2 = 0.03 + wave2Amplitude * sin(iTime * 3.5 + worldPosition.z * 3);
        worldPosition.y += wave1 + wave2; // Add a sine wave effect to the y-coordinate
        float dYdx = wave1Amplitude * 1.5 * cos(iTime * 2.0 + worldPosition.x * 1.5);
        float dYdz = wave2Amplitude * 3.0 * cos(iTime * 3.5 + worldPosition.z * 3);
        vNormal = normalize(vec3(-dYdx, 1.0, -dYdz)); // Calculate the normal based on the wave derivatives
    }
    else
    {
        mat3 normalMatrix = transpose(inverse(mat3(uModel)));
        vNormal = normalize(normalMatrix * inNormal);
    }

    gl_Position = uProjection * uView * worldPosition;
}