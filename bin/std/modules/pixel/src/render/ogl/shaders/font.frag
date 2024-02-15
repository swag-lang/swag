#version 330 core
#include base

in vec4     vcolor;
in vec2     vuv0;
in vec2     vuv1;

uniform sampler2D   inTexture0;
uniform sampler2D   inTexture1;

out vec4 color;

void main()
{
    color = vcolor * texture(inTexture0, vuv0);
    color.w *= texture(inTexture1, vuv1).r;
    //color.w = pow(color.w, 1.0/0.75);
}