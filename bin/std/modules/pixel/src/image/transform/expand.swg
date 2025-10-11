using Core

impl Image
{
    mtd const getWH(l, t, r, b: s32)->{ w, h: s32 }
    {
        let cw = .width + l + r
        let ch = .height + t + b
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

        var srcW = .width8
        if l < 0 do
            srcW += l * .bpp8
        if r < 0 do
            srcW += r * .bpp8

        var srcH = .height
        if t < 0 do
            srcH += t
        if b < 0 do
            srcH += b

        var dstPix = dstImage.pixels.buffer + dstY * dstImage.width8
        dstPix += dstX * .bpp8

        for cy in srcH
        {
            var srcPix = .pixels.buffer + (srcY + cy) * .width8
            srcPix += cast(u64) (srcX * .bpp8)
            Memory.copy(dstPix, srcPix, srcW)
            dstPix += dstImage.width8
        }
    }

    // Crop image
    #[Swag.Overload]
    public mtd expand(l, t, r, b: s32, color: Color = Argb.White)
    {
        let (cw, ch) = .getWH(l, t, r, b)
        .allocWorkingBuffer(cw, ch, .pf)
        .doExpand(.workingBuffer, l, t, r, b, color)
        .applyWorkingBuffer()
    }

    // Crop image
    #[Swag.Overload]
    public mtd const toExpand(l, t, r, b: s32, color: Color = Argb.White)->Image
    {
        let (cw, ch) = .getWH(l, t, r, b)
        var result: retval = create(cw, ch, .pf)
        .doExpand(&result, l, t, r, b, color)
        return result
    }
}
