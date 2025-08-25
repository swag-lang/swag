#global marked
using Core

impl Image
{
    mtd const doFlip(dstImage: *Image)
    {
        let f = dstImage
        me.visitPixels(f, stride: me.width)
        {
            let dstImage = cast(*Image) userData
            let w8       = image.width * image.bpp8
            Memory.copy(dstImage.pixels.buffer + (image.height - y - 1) * w8, pix, cast(u64) w8)
        }
    }

    // Flip image vertically
    public mtd flip()
    {
        Debug.assert(me.pixels.buffer != null)
        me.allocWorkingBuffer()
        me.doFlip(me.workingBuffer)
        me.applyWorkingBuffer()
    }

    // Flip image vertically
    public mtd const toFlip()->Image
    {
        Debug.assert(me.pixels.buffer != null)
        var result: retval = create(me.width, me.height, me.pf)
        me.doFlip(&result)
        return result
    }
}
