#version 330 core
#include base

uniform sampler2D inTexture0;

in vec4 vcolor;
in vec2 vuv0;

out vec4 color;

void main()
{
    color = vcolor * texture(inTexture0, vuv0);
}