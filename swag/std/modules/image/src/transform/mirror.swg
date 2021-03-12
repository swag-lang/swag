using core

impl Image
{
    public func mirrorTo(using self, dstImage: *Image)
    {
        debug.assert(pixels != null)
        f := dstImage

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels + (image.width - x - 1) * image.bpp8
                pixDst += y * (image.width * image.bpp8)
                copyPixel'BPP(pixDst, pix)
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }
    }

    public func mirror(using self)
    {
        debug.assert(pixels != null)
        allocWorkingBuffer()
        mirrorTo(workingBuffer)
        applyWorkingBuffer()
    }
}