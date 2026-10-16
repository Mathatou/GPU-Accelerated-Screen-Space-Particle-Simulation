#version 450
in layout(location = 0) vec3 inPosition;
in layout(location = 1) vec3 inNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vNormal;


void main()
{
    vec4 worldPosition = uModel * vec4(inPosition, 1.0);
    gl_Position = uProjection * uView * worldPosition;

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    vNormal = normalize(normalMatrix * inNormal);
}