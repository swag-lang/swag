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
