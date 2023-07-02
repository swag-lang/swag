using Core

impl Image
{
    mtd const doCrop(dstImage: *Image, x, y, w, h: s32)
    {
        f := {x, y: height - y - h, self}
        dstImage.visitPixels(&f, stride: w)
        {
            params := cast(*@decltype(f)) userData
            srcPix := params.self.pixels.buffer + (y + params.y) * params.self.width8
            srcPix += cast(u64) (params.x * image.bpp8)
            Memory.copy(pix, srcPix, cast(u64) image.width8)
        }
    }

    // Crop image
    public mtd crop(x, y, w, h: s32)
    {
        cx := Math.clamp(x, 0, width - 1)
        cy := Math.clamp(y, 0, height - 1)
        cw := Math.min(w, width - cx)
        ch := Math.min(h, height - cy)
        Debug.assert(cw > 0 and ch > 0)

        allocWorkingBuffer(cw, ch, pf)
        doCrop(workingBuffer, cx, cy, cw, ch)
        applyWorkingBuffer()
    }

    public mtd crop(cropRect: Math.Rectangle)
    {
        return crop(acast cropRect.x, acast cropRect.y, acast cropRect.width, acast cropRect.height)
    }

    // Crop image
    public mtd const toCrop(x, y, w, h: s32)->Image
    {
        cx := Math.clamp(x, 0, width - 1)
        cy := Math.clamp(y, 0, height - 1)
        cw := Math.min(w, width - cx)
        ch := Math.min(h, height - cy)
        Debug.assert(cw > 0 and ch > 0)

        var result: retval = create(cw, ch, pf)
        doCrop(&result, cx, cy, cw, ch)
        return result
    }

    public mtd const toCrop(cropRect: Math.Rectangle)->Image
    {
        return toCrop(acast cropRect.x, acast cropRect.y, acast cropRect.width, acast cropRect.height)
    }
}