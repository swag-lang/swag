using Core

impl Image
{
    mtd doMirror(dstImage: *Image)
    {
        f := dstImage

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            visitPixels(f)
            {
                dstImage := cast(*Image) userData
                pixDst := dstImage.pixels.buffer + (image.width - x - 1) * image.bpp8
                pixDst += cast(uint) (y * (image.width * image.bpp8))
                copyPixel'BPP(pixDst, pix)
            }
        }

        switch bpp
        {
        case 24: doIt'24()
        case 32: doIt'32()
        default: Debug.assert(false)
        }
    }

    // Flip image horizontally
    public mtd mirror()
    {
        Debug.assert(pixels.buffer != null)
        allocWorkingBuffer()
        doMirror(workingBuffer)
        applyWorkingBuffer()
    }

    // Flip image horizontally
    public mtd toMirror()->Image
    {
        Debug.assert(pixels.buffer != null)
        var result: retval = create(width, height, pf)
        doMirror(&result)
        return result
    }
}