using core

impl Image
{
    // Flip image vertically
    public func flipTo(using self, dstImage: *Image)
    {
        debug.assert(pixels != null)

        f := dstImage
        visitPixels(f, stride: width)
        {
            dstImage := cast(*Image) userData
            w8 := image.width * image.bpp8
            memory.copy(dstImage.pixels + (image.height - y - 1) * w8, pix, cast(uint) w8)
        }
    }

    // Flip image vertically
    public func flip(using self)
    {
        debug.assert(pixels != null)
        allocWorkingBuffer()
        flipTo(workingBuffer)
        applyWorkingBuffer()
    }
}