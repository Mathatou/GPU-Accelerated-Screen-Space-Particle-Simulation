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
        float wave1 = 0.07 +0.07 * sin(iTime * 2.0 + worldPosition.x * 1.5);
        float wave2 = 0.03 +0.07 * sin(iTime * 3.5 + worldPosition.z * 3);
        worldPosition.y += wave1 + wave2; // Add a sine wave effect to the y-coordinate
        float dYdx = 0.07 +0.07* 1.5 * cos(iTime * 2.0 + worldPosition.x * 1.5);
        float dYdz = 0.03 +0.07* 3.0 * cos(iTime * 3.5 + worldPosition.z * 3);
        vNormal = normalize(vec3(-dYdx, 1.0, -dYdz)); // Calculate the normal based on the wave derivatives
    }
    else
    {
        mat3 normalMatrix = transpose(inverse(mat3(uModel)));
        vNormal = normalize(normalMatrix * inNormal);
    }

    gl_Position = uProjection * uView * worldPosition;
}