#global public
using Pixel

struct ImageList
{
    texture:         Texture
    totalFrames:     s32
    frameSizeX:      s32
    frameSizeY:      s32
}

impl ImageList
{
    #[Swag.Inline]
    {
        mtd countX() => .texture.width / .frameSizeX
        mtd countY() => .texture.height / .frameSizeY
    }

    // Initialize image list
    mtd set(texture: Texture, fsx, fsy: s32 = 0, cpt: s32 = 0)
    {
        .texture     = texture
        .frameSizeX  = fsx orelse texture.width
        .frameSizeY  = fsy orelse texture.height
        .totalFrames = cpt orelse (.texture.width / .frameSizeX) * (.texture.height / .frameSizeY)
    }
}
