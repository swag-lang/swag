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
    mtdc isValid() => imageList != null

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
        self.size = #scopefct.size orelse cast(f32) self.imageList.sizeX
    }

    mtdc paint(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        if !imageList
            return

        srcX := index % (imageList.texture.width / imageList.sizeX)
        srcY := index / (imageList.texture.height / imageList.sizeY)
        srcX *= imageList.sizeX
        srcY *= imageList.sizeY

        if size == imageList.sizeX and size == imageList.sizeY
            painter.setInterpolationMode(InterpolationMode.Pixel)
        else
            painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture(@{x, y, size, size}, @{srcX, srcY, imageList.sizeX, imageList.sizeY}, imageList.texture, color)
    }
}