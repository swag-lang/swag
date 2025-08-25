#global public
using Pixel

struct Icon
{
    imageList:     *ImageList
    index:         s32
    sizeX:         f32 = 24
    sizeY:         f32 = 24
}

impl Icon
{
    #[Swag.Inline]
    mtd const isValid() => me.imageList != null

    // Set icon to invalid
    mtd clear()
    {
        me.imageList = null
    }

    // Initialize the icon from an image list
    func from(imageList: *ImageList, index: s32, sizeX, sizeY: f32 = 0)->Icon
    {
        var result: retval
        let sx      = sizeX orelse cast(f32) imageList.frameSizeX
        let sy      = sizeY orelse cast(f32) imageList.frameSizeY
        result.set(imageList, index, sx, sy)
        return result
    }

    // Initialize the icon from an image list
    mtd set(imageList: *ImageList, index: s32, sizeX, sizeY: f32 = 0)
    {
        me.imageList = #me.imageList
        me.index     = #me.index
        let sx = #me.sizeX orelse cast(f32) #me.imageList.frameSizeX
        let sy = #me.sizeY orelse cast(f32) #me.imageList.frameSizeY
        me.sizeX = sx
        me.sizeY = sy
    }

    // Paint icon at the given position
    mtd const paint(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        if !me.imageList or !me.imageList.totalFrames:
            return

        let idx  = me.index % me.imageList.totalFrames
        var srcX = idx % me.imageList.countX()
        var srcY = idx / me.imageList.countX()

        srcX *= me.imageList.frameSizeX
        srcY *= me.imageList.frameSizeY

        if me.sizeX == me.imageList.frameSizeX and me.sizeY == me.imageList.frameSizeY:
            painter.setInterpolationMode(InterpolationMode.Pixel)
        else:
            painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture({x, y, me.sizeX, me.sizeY}, {srcX, srcY, me.imageList.frameSizeX, me.imageList.frameSizeY}, me.imageList.texture, color)
    }
}
