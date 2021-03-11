using core

impl Image
{
    public enum ResizeMethod
    {
        Raw
    }

    func resizeRaw(using self)
    {
        xscale := wbWidth / cast(f32) width
        yscale := wbHeight / cast(f32) height
        f := @{xscale, yscale}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := image.workingBuffer + (fx * image.bpp8) + (fy * image.wbWidth * image.bpp8)
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

        allocWorkingBuffer(newWidth, newHeight, bpp8)
        applyWorkingBuffer()

        #[swag.complete]
        switch method
        {
        case ResizeMethod.Raw: resizeRaw()
        }
    }
}