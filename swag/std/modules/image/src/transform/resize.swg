using core

impl Image
{
    public enum ResizeMethod
    {
        Raw
    }

    private func doResizeRaw(using self, dstImage: *Image)
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

    private func doResize(using self, dstImage: *Image, method: ResizeMethod)
    {
        #[swag.complete]
        switch method
        {
        case Raw: doResizeRaw(dstImage)
        }
    }

    // Resize image
    public func resize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        if newWidth == width && newHeight == height return

        allocWorkingBuffer(newWidth, newHeight, pf)
        doResize(workingBuffer, method)
        applyWorkingBuffer()
    }

    // Resize image
    public func toResize(using self, newWidth, newHeight: u32, method = ResizeMethod.Raw)->Image
    {
        debug.assert(newWidth > 0 && newHeight > 0)

        if newWidth == width && newHeight == height
        {
            var result: retval = :self
            return result
        }

        var result: retval = create(newWidth, newHeight, pf)
        doResize(&result, method)
        return result
    }
}