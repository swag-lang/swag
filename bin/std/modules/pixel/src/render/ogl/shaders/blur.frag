#version 330 core
#include base

uniform sampler2D inTexture0;

in vec4 vcolor;
in vec2 vuv0;

out vec4 color;

uniform float radius = 4;     // blur radius
uniform int   axis = 0;

void main()
{
    vec2  pos = vuv0;
    pos *= 2;
    pos -= vec2(1, 1);

    float x, y;
    float rr = radius*radius;
    float d, w, w0;
    vec2  p = 0.5*(vec2(1.0,1.0)+pos);

    color = vec4(0.0,0.0,0.0,0.0);

    w0 = 0.5135 / pow(radius, 0.96);

    if(axis == 0)
    {
        for (d = 1.0 / textureW, x = -radius, p.x += x*d; x <= radius; x++, p.x += d)
        {
            w = w0 * exp((-x*x) / (2.0*rr));
            color += texture(inTexture0, p) * w;
        }
    }
    else
    {
        for (d = 1.0 / textureH, y = -radius, p.y += y*d; y <= radius; y++, p.y += d)
        {
            w = w0 * exp((-y*y) / (2.0*rr));
            color += texture(inTexture0, p) * w;
        }
    }

    color *= vcolor;
}