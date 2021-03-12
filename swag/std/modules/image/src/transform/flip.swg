using core

impl Image
{
    private func flipTo(using self, dstImage: *Image)
    {
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

    // Flip image vertically
    public func getFlip(using self)->Image
    {
        debug.assert(pixels != null)
        var result: retval = create(width, height, pf)
        flipTo(&result)
        return result
    }
}