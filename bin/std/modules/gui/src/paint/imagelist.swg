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
        mtd countX() => texture.width / frameSizeX
        mtd countY() => texture.height / frameSizeY
    }

    // Initialize image list
    mtd set(texture: Texture, fsx, fsy: s32 = 0, cpt: s32 = 0)
    {
        me.texture  = #me.texture
        frameSizeX  = fsx orelse #me.texture.width
        frameSizeY  = fsy orelse #me.texture.height
        totalFrames = cpt orelse (me.texture.width / frameSizeX) * (me.texture.height / frameSizeY)
    }
}
