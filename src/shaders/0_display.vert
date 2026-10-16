#version 450
out vec2 vTexCoord;

void drawQuad()
{
    vec2 lQuadVertices[4] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 1.0, -1.0),
        vec2(-1.0,  1.0),
        vec2( 1.0,  1.0)
    );
    gl_Position = vec4(lQuadVertices[gl_VertexID], 0.0, 1.0);
    vTexCoord = lQuadVertices[gl_VertexID] * 0.5 + 0.5; // From NDC to Texture coordinates
}


void main()
{
    drawQuad();
}