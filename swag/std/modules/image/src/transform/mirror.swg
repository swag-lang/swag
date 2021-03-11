using core

impl Image
{
    // Flip image horizontally
    public func mirror(using self)
    {
        debug.assert(pixels != null)

        allocWorkingBuffer()
        switch bpp
        {
        case 24:
            visitPixels()
            {
                pixDst := image.workingBuffer + (image.width - x - 1) * image.bpp8
                pixDst += y * (image.width * image.bpp8)
                :cast(*u16) pixDst = :cast(*u16) pix
                pixDst[2] = pix[2]
            }
        case 32:
            visitPixels()
            {
                pixDst := image.workingBuffer + (image.width - x - 1) * image.bpp8
                pixDst += y * (image.width * image.bpp8)
                :cast(*u32) pixDst = :cast(*u32) pix
            }
        }

        applyWorkingBuffer()
    }
}