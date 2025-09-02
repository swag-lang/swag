using Core

impl Image
{
    mtd const getWH(l, t, r, b: s32)->{ w, h: s32 }
    {
        let cw = me.width + l + r
        let ch = me.height + t + b
        Debug.assert(cw > 0 and ch > 0)
        return {cw, ch}
    }

    mtd const doExpand(dstImage: *Image, l, t, r, b: s32, color: Color)
    {
        dstImage.fill(color)

        let dstX = l < 0 ? 0 : l
        let dstY = t < 0 ? 0 : t
        let srcX = l < 0 ? -l : 0
        let srcY = t < 0 ? -t : 0

        var srcW = me.width8
        if l < 0:
            srcW += l * me.bpp8
        if r < 0:
            srcW += r * me.bpp8

        var srcH = me.height
        if t < 0:
            srcH += t
        if b < 0:
            srcH += b

        var dstPix = dstImage.pixels.buffer + dstY * dstImage.width8
        dstPix += dstX * me.bpp8

        for cy in srcH
        {
            var srcPix = me.pixels.buffer + (srcY + cy) * me.width8
            srcPix += cast(u64) (srcX * me.bpp8)
            Memory.copy(dstPix, srcPix, srcW)
            dstPix += dstImage.width8
        }
    }

    // Crop image
    #[Swag.Overload]
    public mtd expand(l, t, r, b: s32, color: Color = Argb.White)
    {
        let (cw, ch) = me.getWH(l, t, r, b)
        me.allocWorkingBuffer(cw, ch, me.pf)
        me.doExpand(me.workingBuffer, l, t, r, b, color)
        me.applyWorkingBuffer()
    }

    // Crop image
    #[Swag.Overload]
    public mtd const toExpand(l, t, r, b: s32, color: Color = Argb.White)->Image
    {
        let (cw, ch) = me.getWH(l, t, r, b)
        var result: retval = create(cw, ch, me.pf)
        me.doExpand(&result, l, t, r, b, color)
        return result
    }
}
