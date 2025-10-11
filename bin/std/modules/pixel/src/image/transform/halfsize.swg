using Core

impl Image
{
    mtd const doHalfSize(dstImage: *Image)
    {
        var f = {img: me}
        dstImage.visitPixels(&f)
        {
            let data       = cast(*#decltype(f)) userData
            let srcImage   = cast(const *Image) data.img
            let b8         = srcImage.bpp8
            let w8         = srcImage.width * b8
            let pixSrc     = srcImage.pixels.buffer + (x * 2 * b8) + (y * 2 * w8)
            let pixSrcb8   = pixSrc + b8
            let pixSrcw8   = pixSrc + w8
            let pixSrcw8b8 = pixSrcw8 + b8
            pix[0] = cast(u8) ((pixSrc[0] + #prom pixSrcb8[0] + #prom pixSrcw8[0] + #prom pixSrcw8b8[0]) >> 2)
            pix[1] = cast(u8) ((pixSrc[1] + #prom pixSrcb8[1] + #prom pixSrcw8[1] + #prom pixSrcw8b8[1]) >> 2)
            pix[2] = cast(u8) ((pixSrc[2] + #prom pixSrcb8[2] + #prom pixSrcw8[2] + #prom pixSrcw8b8[2]) >> 2)
            if image.bpp == 32 do
                pix[3] = cast(u8) ((pixSrc[3] + #prom pixSrcb8[3] + #prom pixSrcw8[3] + #prom pixSrcw8b8[3]) >> 2)
        }
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public mtd halfSize()
    {
        Debug.assert(.width > 1 and .height > 1)
        .allocWorkingBuffer(.width / 2, .height / 2, .pf)
        .doHalfSize(.workingBuffer)
        .applyWorkingBuffer()
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public mtd const toHalfSize()->Image
    {
        Debug.assert(.width > 1 and .height > 1)
        var result: retval = create(.width / 2, .height / 2, .pf)
        .doHalfSize(&result)
        return result
    }
}
