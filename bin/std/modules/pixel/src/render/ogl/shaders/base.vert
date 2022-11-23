layout(location = 0)  in vec2 vertexPosition;
layout(location = 1)  in vec2 uv1;
layout(location = 2)  in vec4 vertexColor;
layout(location = 3)  in vec4 aa0;
layout(location = 4)  in vec4 aa1;
layout(location = 5)  in vec4 aa2;
layout(location = 6)  in vec4 aa3;
layout(location = 7)  in vec4 aa4;
layout(location = 8)  in vec4 aa5;
layout(location = 9)  in vec4 aa6;
layout(location = 10) in vec4 aa7;
layout(location = 11) in vec4 aa8;
layout(location = 12) in vec4 aa9;
layout(location = 13) in vec4 aa10;
layout(location = 14) in vec4 aa11;
layout(location = 15) in float aanum;

uniform mat4    mvp;
uniform mat4    mdl;
uniform vec4    boundRect;
uniform vec4    textureRect;
uniform float   uvMode;

vec2 computeUVs(vec2 pixelPos)
{
    vec2 uv;

    // Fill
    if(uvMode < 0.5)
    {
        float w = boundRect.z - boundRect.x;
        float h = boundRect.w - boundRect.y;
        uv.x = (pixelPos.x - boundRect.x) / w;
        uv.y = 1 - (pixelPos.y - boundRect.y) / h;
    }

    // FillSubRect
    else if(uvMode < 1.5)
    {
        float w = boundRect.z - boundRect.x;
        float h = boundRect.w - boundRect.y;
        float xt = (pixelPos.x - boundRect.x) / w;
        float yt = (pixelPos.y - boundRect.y) / h;
        uv.x = textureRect.x + (xt * (textureRect.z - textureRect.x));
        uv.y = 1 - (textureRect.y + (yt * (textureRect.w - textureRect.y)));
    }

    // Tile
    else if(uvMode < 2.5)
    {
        uv.x = (pixelPos.x - boundRect.x) / textureRect.z;
        uv.y = 1 - (pixelPos.y - boundRect.y) / textureRect.w;
    }

    return uv;
}
