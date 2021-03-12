using core

impl Image
{
    // Resize image
    public func halfSize(using self)
    {
        debug.assert(width > 1 && height > 1)

        allocWorkingBuffer(width / 2, height / 2, bpp8)
        applyWorkingBuffer()

        visitPixels()
        {
            b8 := image.bpp8
            w8 := (image.wbWidth * b8)
            pixSrc := image.workingBuffer + (x * 2 * b8) + (y * 2 * w8)
            pix[0] = cast(u8) ((pixSrc[0] + pixSrc[b8]   + pixSrc[w8]   + pixSrc[w8+b8]) >>% 2)
            pix[1] = cast(u8) ((pixSrc[1] + pixSrc[b8+1] + pixSrc[w8+1] + pixSrc[w8+b8+1]) >>% 2)
            pix[2] = cast(u8) ((pixSrc[2] + pixSrc[b8+2] + pixSrc[w8+2] + pixSrc[w8+b8+2]) >>% 2)
        }
    }
}