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
        mtd countX() => me.texture.width / me.frameSizeX
        mtd countY() => me.texture.height / me.frameSizeY
    }

    // Initialize image list
    mtd set(texture: Texture, fsx, fsy: s32 = 0, cpt: s32 = 0)
    {
        me.texture     = #me.texture
        me.frameSizeX  = fsx orelse #me.texture.width
        me.frameSizeY  = fsy orelse #me.texture.height
        me.totalFrames = cpt orelse (me.texture.width / me.frameSizeX) * (me.texture.height / me.frameSizeY)
    }
}
