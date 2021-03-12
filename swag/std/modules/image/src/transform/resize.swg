using core

impl Image
{
    public enum ResizeMethod
    {
        Raw
    }

    private func resizeRaw(using self, srcImage: *Image)
    {
        xscale := srcImage.width / cast(f32) width
        yscale := srcImage.height / cast(f32) height
        f := @{xscale, yscale, srcImage}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := params.srcImage.pixels + (fx * image.bpp8) + (fy * params.srcImage.width * image.bpp8)
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

        allocWorkingBuffer(newWidth, newHeight, pf)

        #[swag.complete]
        switch method
        {
        case Raw: workingBuffer.resizeRaw(self)
        }

        applyWorkingBuffer()
    }
}