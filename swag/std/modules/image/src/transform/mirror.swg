using core

impl Image
{
    // Flip image horizontally
    public func mirror(using self)
    {
        debug.assert(pixels != null)

        #[swag.mixin]
        func(BPP: u8) doIt()
        {
            visitPixels()
            {
                pixDst := image.workingBuffer + (image.width - x - 1) * image.bpp8
                pixDst += y * (image.width * image.bpp8)
                pixelCopy'BPP(pixDst, pix)
            }
        }

        allocWorkingBuffer()
        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: debug.assert(false)
        }

        applyWorkingBuffer()
    }
}