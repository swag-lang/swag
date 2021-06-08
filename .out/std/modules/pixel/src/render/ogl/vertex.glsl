#version 330 core

layout(location = 0) in vec2 vertexPosition;
layout(location = 1) in vec2 uv1;
layout(location = 2) in vec4 vertexColor;
layout(location = 3) in vec4 aa0;
layout(location = 4) in vec4 aa1;
layout(location = 5) in vec4 aa2;
layout(location = 6) in vec4 aa3;
layout(location = 7) in vec4 aa4;
layout(location = 8) in vec4 aa5;
layout(location = 9) in float aanum;

uniform mat4    mvp;
uniform mat4    mdl;
uniform float   uvmode;
uniform vec4    boundRect;
uniform vec4    textureRect;

out vec4        vcolor;
out vec2        vuv0;
out vec2        vuv1;
out vec4        vaa0;
out vec4        vaa1;
out vec4        vaa2;
out vec4        vaa3;
out vec4        vaa4;
out vec4        vaa5;
out float       vaanum;

vec2 computeUVs(vec2 pixelPos)
{
    vec2 uv;

    // Fill
    if(uvmode < 0.5)
    {
        float w = boundRect.z - boundRect.x;
        float h = boundRect.w - boundRect.y;
        uv.x = (pixelPos.x - boundRect.x) / w;
        uv.y = 1 - (pixelPos.y - boundRect.y) / h;
    }

    // FillSubRect
    else if(uvmode < 1.5)
    {
        float w = boundRect.z - boundRect.x;
        float h = boundRect.w - boundRect.y;
        float xt = (pixelPos.x - boundRect.x) / w;
        float yt = (pixelPos.y - boundRect.y) / h;
        uv.x = textureRect.x + (xt * (textureRect.z - textureRect.x));
        uv.y = 1 - (textureRect.y + (yt * (textureRect.w - textureRect.y)));
    }

    // Tile
    else if(uvmode < 2.5)
    {
        uv.x = (pixelPos.x - boundRect.x) / textureRect.z;
        uv.y = 1 - (pixelPos.y - boundRect.y) / textureRect.w;
    }

    return uv;
}

vec4 computeTransformAA(vec4 aa)
{
    vec4 v0 = mdl * vec4(aa.xy, 0, 1);
    vec4 v1 = mdl * vec4(aa.zw, 0, 1);
    return vec4(v0.xy, v1.xy);
}

void transformAA()
{
    vaanum = aanum;
    if(vaanum < 0.5) return;
    vaa0 = computeTransformAA(aa0);
    if(vaanum < 1.5) return;
    vaa1 = computeTransformAA(aa1);
    if(vaanum < 2.5) return;
    vaa2 = computeTransformAA(aa2);
    if(vaanum < 3.5) return;
    vaa3 = computeTransformAA(aa3);
    if(vaanum < 4.5) return;
    vaa4 = computeTransformAA(aa4);
    if(vaanum < 5.5) return;
    vaa5 = computeTransformAA(aa5);
}

void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);

    // source is rgba, and we need bgra
    vcolor = vertexColor.zyxw;

    // uv
    vuv0 = computeUVs(vertexPosition.xy);
    vuv1.x = uv1.x;
    vuv1.y = 1 - uv1.y;

    transformAA();
}