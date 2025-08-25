using Core

impl Image
{
    mtd const doCrop(dstImage: *Image, x, y, w, h: s32)
    {
        var f = {x, y: me.height - y - h, me}
        dstImage.visitPixels(&f, stride: w)
        {
            let params = cast(*#decltype(f)) userData
            var srcPix = params.me.pixels.buffer + (y + params.y) * params.me.width8
            srcPix += cast(u64) (params.x * image.bpp8)
            Memory.copy(pix, srcPix, cast(u64) image.width8)
        }
    }

    // Crop image
    #[Swag.Overload]
    public mtd crop(x, y, w, h: s32)
    {
        let cx = Math.clamp(x, 0, me.width - 1)
        let cy = Math.clamp(y, 0, me.height - 1)
        let cw = Math.min(w, me.width - cx)
        let ch = Math.min(h, me.height - cy)
        Debug.assert(cw > 0 and ch > 0)

        me.allocWorkingBuffer(cw, ch, me.pf)
        me.doCrop(me.workingBuffer, cx, cy, cw, ch)
        me.applyWorkingBuffer()
    }

    #[Swag.Overload]
    public mtd crop(cropRect: Math.Rectangle)
    {
        return me.crop(cast() cropRect.x, cast() cropRect.y, cast() cropRect.width, cast() cropRect.height)
    }

    // Crop image
    #[Swag.Overload]
    public mtd const toCrop(x, y, w, h: s32)->Image
    {
        let cx = Math.clamp(x, 0, me.width - 1)
        let cy = Math.clamp(y, 0, me.height - 1)
        let cw = Math.min(w, me.width - cx)
        let ch = Math.min(h, me.height - cy)
        Debug.assert(cw > 0 and ch > 0)

        var result: retval = create(cw, ch, me.pf)
        me.doCrop(&result, cx, cy, cw, ch)
        return result
    }

    #[Swag.Overload]
    public mtd const toCrop(cropRect: Math.Rectangle)->Image
    {
        return me.toCrop(cast() cropRect.x, cast() cropRect.y, cast() cropRect.width, cast() cropRect.height)
    }
}
