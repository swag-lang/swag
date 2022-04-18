#global public
using Pixel

struct Icon
{
    imageList:  *ImageList
    index:      s32
    sizeX:      f32 = 24
    sizeY:      f32 = 24
}

impl Icon
{
    #[Swag.Inline]
    mtdc isValid() => imageList != null

    // Set icon to invalid
    mtd clear()
    {
        imageList = null
    }

    // Initialize the icon from an image list
    func from(imageList: *ImageList, index: s32, sizeX, sizeY: f32 = 24)->Icon
    {
        var result: retval
        result.set(imageList, index, sizeX, sizeY)
        return result
    }

    // Initialize the icon from an image list
    mtd set(imageList: *ImageList, index: s32, sizeX, sizeY: f32 = 24)
    {
        self.imageList = #self.imageList
        self.index     = #self.index
        self.sizeX     = #self.sizeX
        self.sizeY     = #self.sizeY
    }

    // Paint icon at the given position
    mtdc paint(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        if !imageList
            return

        srcX := index % imageList.countX()
        srcY := index / imageList.countX()

        srcX *= imageList.frameSizeX
        srcY *= imageList.frameSizeY

        if sizeX == imageList.frameSizeX and sizeY == imageList.frameSizeY
            painter.setInterpolationMode(InterpolationMode.Pixel)
        else
            painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture(@{x, y, sizeX, sizeY}, @{srcX, srcY, imageList.frameSizeX, imageList.frameSizeY}, imageList.texture, color)
    }
}