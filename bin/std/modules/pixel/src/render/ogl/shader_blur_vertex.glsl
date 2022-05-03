#version 120

in vec2 vertexPosition;

uniform mat4 mvp;
uniform mat4 mdl;

void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);
    gl_TexCoord[0] = gl_MultiTexCoord0;
}