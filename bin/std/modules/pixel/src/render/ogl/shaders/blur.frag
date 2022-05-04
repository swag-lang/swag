#version 330 core
#include base

uniform sampler2D inTexture0;

in vec2 vuv0;

out vec4 color;

uniform float radius = 4;     // blur radius
uniform int   axis = 0;

void main()
{
    //color = texture(inTexture0, vuv0);
    //color.xyz *= 0.7;

    float r = 4;
    vec2  pos = vuv0;
    pos *= 2;
    pos -= vec2(1, 1);

    float x,y;
    float rr = r*r;
    float d,w,w0;
    vec2  p=0.5*(vec2(1.0,1.0)+pos);
    vec4  col=vec4(0.0,0.0,0.0,0.0);

    w0 = 0.5135 / pow(r,0.96);
    if (axis==0)
    {
        for (d=1.0/textureW,x=-r,p.x+=x*d;x<=r;x++,p.x+=d)
        {
            w = w0 * exp((-x*x) / (2.0*rr));
            col += texture(inTexture0, p) * w;
        }
    }
    else if (axis==1)
    {
        for (d=1.0/textureH,y=-r,p.y+=y*d;y<=r;y++,p.y+=d)
        {
            w = w0 * exp((-y*y) / (2.0*rr));
            col += texture(inTexture0, p) * w;
        }
    }

    color = col;
}