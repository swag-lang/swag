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
        self.texture = #self.texture
        frameSizeX   = fsx orelse #self.texture.width
        frameSizeY   = fsy orelse #self.texture.height
        totalFrames  = cpt orelse (self.texture.width / frameSizeX) * (self.texture.height / frameSizeY)
    }
}
