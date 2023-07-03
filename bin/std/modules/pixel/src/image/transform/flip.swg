using Core

impl Image
{
    mtd const doFlip(dstImage: *Image)
    {
        f := dstImage
        visitPixels(f, stride = width)
        {
            dstImage := cast(*Image) userData
            w8 := image.width * image.bpp8
            Memory.copy(dstImage.pixels.buffer + (image.height - y - 1) * w8, pix, cast(u64) w8)
        }
    }

    // Flip image vertically
    public mtd flip()
    {
        Debug.assert(pixels.buffer != null)
        allocWorkingBuffer()
        doFlip(workingBuffer)
        applyWorkingBuffer()
    }

    // Flip image vertically
    public mtd const toFlip()->Image
    {
        Debug.assert(pixels.buffer != null)
        var result: retval = create(width, height, pf)
        doFlip(&result)
        return result
    }
}