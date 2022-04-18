#global public
using Pixel

struct ImageList
{
    texture:    Texture
    sizeX:      s32
    sizeY:      s32
}

impl ImageList
{
    #[Swag.Inline]
    {
        mtd countX() => cast(s32) (texture.width / sizeX)
        mtd countY() => cast(s32) (texture.height / sizeY)
    }
}