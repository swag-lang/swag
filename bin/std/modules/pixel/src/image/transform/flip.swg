using Core

impl Image
{
    private mtd doFlip(dstImage: *Image)
    {
        f := dstImage
        visitPixels(f, stride: width)
        {
            dstImage := cast(*Image) userData
            w8 := image.width * image.bpp8
            Memory.copy(dstImage.pixels + (image.height - y - 1) * w8, pix, cast(uint) w8)
        }
    }

    // Flip image vertically
    public mtd flip()
    {
        Debug.assert(pixels != null)
        allocWorkingBuffer()
        doFlip(workingBuffer)
        applyWorkingBuffer()
    }

    // Flip image vertically
    public mtd toFlip()->Image
    {
        Debug.assert(pixels != null)
        var result: retval = create(width, height, pf)
        doFlip(&result)
        return result
    }
}