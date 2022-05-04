#version 330 core
#include base

out vec2 vuv0;

void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);
    vuv0 = computeUVs(vertexPosition.xy);
}