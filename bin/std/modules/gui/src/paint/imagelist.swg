#global public
using Pixel

struct ImageList
{
    texture:    Texture
    frameSizeX: s32
    frameSizeY: s32
}

impl ImageList
{
    #[Swag.Inline]
    {
        mtd countX() => texture.width / frameSizeX
        mtd countY() => texture.height / frameSizeY
    }
}