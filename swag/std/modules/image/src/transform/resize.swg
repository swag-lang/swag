using core

impl Image
{
    public enum ResizeMethod
    {
        Raw
    }

    func resizeRaw(using self, newWidth, newHeight: u32)
    {
        xscale := width / cast(f32) newWidth
        yscale := height / cast(f32) newHeight

        srcWidth := width
        srcHeight := height

        // Current image is now the computing buffer
        applyWorkingBuffer()
        width = newWidth
        height = newHeight
        size = newWidth * newHeight * bpp8

        f := @{xscale, yscale, srcWidth}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.item0)
                fy := cast(u32) (y * params.item1)
                pixSrc := image.workingBuffer + (fx * image.bpp8) + (fy * params.item2 * image.bpp8)
                copyPixel'BPP(pix, pixSrc)
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    // Resize image
    public func resize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)
        if newWidth == width && newHeight == height return

        // Computing buffer
        newSize := cast(uint) newWidth * newHeight * bpp8
        allocWorkingBuffer(newSize)

        using ResizeMethod
        #[swag.complete]
        switch method
        {
        case Raw:
            resizeRaw(newWidth, newHeight)
        }
    }
}