#version 330 core

uniform sampler2D inTexture0;

in  vec2 vuv0;
out vec4 color;

void main()
{
    color = texture(inTexture0, vuv0);
    color.xyz *= 0.7;
}