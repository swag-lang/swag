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
            w8 := image.wbWidth * b8
            pixSrc := image.workingBuffer + (x * 2 * b8) + (y * 2 * w8)
            pixSrcb8 := pixSrc + b8
            pixSrcw8 := pixSrc + w8
            pixSrcw8b8 := pixSrc + w8 + b8
            pix[0] = cast(u8) ((pixSrc[0] + pixSrcb8[0] + pixSrcw8[0] + pixSrcw8b8[0]) >>% 2)
            pix[1] = cast(u8) ((pixSrc[1] + pixSrcb8[1] + pixSrcw8[1] + pixSrcw8b8[1]) >>% 2)
            pix[2] = cast(u8) ((pixSrc[2] + pixSrcb8[2] + pixSrcw8[2] + pixSrcw8b8[2]) >>% 2)
        }
    }
}