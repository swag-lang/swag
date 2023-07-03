using Core

impl Image
{
    mtd const doHalfSize(dstImage: *Image)
    {
        f := {img = self}
        dstImage.visitPixels(&f)
        {
            data     := cast(*@decltype(f)) userData
            srcImage := cast(const *Image) data.img
            b8 := srcImage.bpp8
            w8 := srcImage.width * b8
            pixSrc := srcImage.pixels.buffer + (x * 2 * b8) + (y * 2 * w8)
            pixSrcb8 := pixSrc + b8
            pixSrcw8 := pixSrc + w8
            pixSrcw8b8 := pixSrcw8 + b8
            pix[0] = cast(u8) ((pixSrc[0] +,up pixSrcb8[0] +,up pixSrcw8[0] +,up pixSrcw8b8[0]) >> 2)
            pix[1] = cast(u8) ((pixSrc[1] +,up pixSrcb8[1] +,up pixSrcw8[1] +,up pixSrcw8b8[1]) >> 2)
            pix[2] = cast(u8) ((pixSrc[2] +,up pixSrcb8[2] +,up pixSrcw8[2] +,up pixSrcw8b8[2]) >> 2)
            if image.bpp == 32 pix[3] = cast(u8) ((pixSrc[3] +,up pixSrcb8[3] +,up pixSrcw8[3] +,up pixSrcw8b8[3]) >> 2)
        }
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public mtd halfSize()
    {
        Debug.assert(width > 1 and height > 1)
        allocWorkingBuffer(width / 2, height / 2, pf)
        doHalfSize(workingBuffer)
        applyWorkingBuffer()
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public mtd const toHalfSize()->Image
    {
        Debug.assert(width > 1 and height > 1)
        var result: retval = create(width / 2, height / 2, pf)
        doHalfSize(&result)
        return result
    }
}