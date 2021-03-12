using core

impl Image
{
    public enum ResizeMethod
    {
        Raw
    }

    private func resizeRaw(using self, dstImage: *Image)
    {
        xscale := width / cast(f32) dstImage.width
        yscale := height / cast(f32) dstImage.height
        f := @{xscale, yscale, self}

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            dstImage.visitPixels(&f)
            {
                params := cast(*@typeof(f)) userData
                fx := cast(u32) (x * params.xscale)
                fy := cast(u32) (y * params.yscale)
                pixSrc := params.self.pixels + (fx * image.bpp8) + (fy * params.self.width * image.bpp8)
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

    public func resizeTo(using self, dstImage: *Image, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        #[swag.complete]
        switch method
        {
        case Raw: resizeRaw(dstImage)
        }
    }

    public func resize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)
        if newWidth == width && newHeight == height return

        allocWorkingBuffer(newWidth, newHeight, pf)
        resizeTo(workingBuffer, newWidth, newHeight, method)
        applyWorkingBuffer()
    }
}