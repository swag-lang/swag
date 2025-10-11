#global public
using Pixel

struct Icon
{
    imageList:     *ImageList
    index:         s32
    sizeX:         f32 = 24
    sizeY:         f32 = 24
    color:         Color
}

impl Icon
{
    #[Swag.Inline]
    mtd const isValid() => .imageList != null

    // Set icon to invalid
    mtd clear()
    {
        .imageList = null
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
        .imageList = imageList
        .index     = index
        let sx = sizeX orelse cast(f32) imageList.frameSizeX
        let sy = sizeY orelse cast(f32) imageList.frameSizeY
        .sizeX = sx
        .sizeY = sy
    }

    // Paint icon at the given position
    mtd const paint(painter: *Painter, x, y: f32, color: Color = Argb.White)
    {
        if !.imageList or !.imageList.totalFrames do
            return

        let idx  = .index % .imageList.totalFrames
        var srcX = idx % .imageList.countX()
        var srcY = idx / .imageList.countX()

        srcX *= .imageList.frameSizeX
        srcY *= .imageList.frameSizeY

        if .sizeX == .imageList.frameSizeX and .sizeY == .imageList.frameSizeY do
            painter.setInterpolationMode(InterpolationMode.Pixel)
        else do
            painter.setInterpolationMode(InterpolationMode.Linear)
        painter.drawTexture({x, y, .sizeX, .sizeY}, {srcX, srcY, .imageList.frameSizeX, .imageList.frameSizeY}, .imageList.texture, color)
    }
}
