using core

impl Image
{
    // Flip image vertically
    public func flip(using self)
    {
        debug.assert(pixels != null)

        allocWorkingBuffer()
        visitPixels(stride: width)
        {
            w8 := image.width * image.bpp8
            memory.copy(image.workingBuffer + (image.height - y - 1) * w8, pix, cast(uint) w8)
        }

        applyWorkingBuffer()
    }
}