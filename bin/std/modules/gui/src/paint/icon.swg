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
    mtd set(imageList: *ImageList, index: s32, size: f32 = 24)
    {
        self.imageList = #scopefct.imageList
        self.index = #scopefct.index
        self.size = #scopefct.size
    }

    mtd draw(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        srcX := index % (imageList.texture.width / imageList.sizeX)
        srcY := index / (imageList.texture.height / imageList.sizeY)
        srcX *= imageList.sizeX
        srcY *= imageList.sizeY

        painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture(@{x, y, size, size}, @{srcX, srcY, imageList.sizeX, imageList.sizeY}, imageList.texture, color)
    }
}