#global public
using Pixel

struct Icon
{
    imageList:  *ImageList
    index:      s32
    size:       f32 = 24
}

impl Icon
{
    #[Swag.Inline]
    mtd isValid() => imageList != null

    func from(imageList: *ImageList, index: s32, size: f32 = 24)->Icon
    {
        var result: retval
        result.set(imageList, index, size)
        return result
    }

    mtd set(imageList: *ImageList, index: s32, size: f32 = 24)
    {
        self.imageList = #scopefct.imageList
        self.index = #scopefct.index
        self.size = #scopefct.size
    }

    mtdc paint(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        if !imageList
            return

        srcX := index % (imageList.texture.width / imageList.sizeX)
        srcY := index / (imageList.texture.height / imageList.sizeY)
        srcX *= imageList.sizeX
        srcY *= imageList.sizeY

        painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture(@{x, y, size, size}, @{srcX + 0.5, srcY + 0.5, imageList.sizeX - 1, imageList.sizeY - 1}, imageList.texture, color)
    }
}