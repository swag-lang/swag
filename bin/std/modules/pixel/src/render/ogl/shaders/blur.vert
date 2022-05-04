#version 330 core
#include baseUniform

out vec2 vuv0;

#include baseFunction


void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);
    vuv0 = computeUVs(vertexPosition.xy);
}