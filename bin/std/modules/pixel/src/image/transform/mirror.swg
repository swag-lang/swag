using Core

impl Image
{
    mtd const doMirror(dstImage: *Image)
    {
        let mec = cast #unconst (*Image) me
        let f   = dstImage

        #[Swag.Mixin]
        func(BPP: u8) doIt()
        {
            mec.visitPixels(f)
            {
                let dstImage = cast(*Image) userData
                var pixDst   = dstImage.pixels.buffer + (image.width - x - 1) * image.bpp8
                pixDst += cast(u64) (y * (image.width * image.bpp8))
                copyPixelDirect'BPP(pixDst, pix)
            }
        }

        switch .bpp
        {
        case 24:
            doIt'24()
        case 32:
            doIt'32()
        default:
            Debug.assert(false)
        }
    }

    // Flip image horizontally
    public mtd mirror()
    {
        Debug.assert(.pixels.buffer != null)
        .allocWorkingBuffer()
        .doMirror(.workingBuffer)
        .applyWorkingBuffer()
    }

    // Flip image horizontally
    public mtd const toMirror()->Image
    {
        Debug.assert(.pixels.buffer != null)
        var result: retval = create(.width, .height, .pf)
        .doMirror(&result)
        return result
    }
}
