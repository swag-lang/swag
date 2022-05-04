#version 330 core

layout(location = 0) in vec2 vertexPosition;

uniform mat4  mvp;
uniform mat4  mdl;
uniform vec4  boundRect;
uniform vec4  textureRect;
uniform float uvMode;
uniform bool  copyMode;

out vec2 vuv0;

vec2 computeUVs(vec2 pixelPos)
{
    float w = boundRect.z - boundRect.x;
    float h = boundRect.w - boundRect.y;
    float xt = (pixelPos.x - boundRect.x) / w;
    float yt = (pixelPos.y - boundRect.y) / h;
    vec2  uv;

    uv.x = textureRect.x + (xt * (textureRect.z - textureRect.x));
    uv.y = 1 - (textureRect.y + (yt * (textureRect.w - textureRect.y)));
    return uv;
}

void main()
{
    gl_Position = mvp * mdl * vec4(vertexPosition, 0, 1);
    vuv0 = computeUVs(vertexPosition.xy);
}