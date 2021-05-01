using core

impl Image
{
    private func doCrop(using self, dstImage: *Image, x, y, w, h: s32)
    {
        f := @{x, y: height - y - h, self}
        dstImage.visitPixels(&f, stride: w)
        {
            params := cast(*@typeof(f)) userData
            srcPix := params.self.pixels + (y + params.y) * params.self.width8
            srcPix += cast(uint) (params.x * image.bpp8)
            memory.copy(pix, srcPix, cast(uint) image.width8)
        }
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public func crop(using self, x, y, w, h: s32)
    {
        cx := math.clamp(x, 0, width - 1)
        cy := math.clamp(y, 0, height - 1)
        cw := math.min(w, width - cx)
        ch := math.min(h, height - cy)
        debug.assert(cw > 0 and ch > 0)

        allocWorkingBuffer(cw, ch, pf)
        doCrop(workingBuffer, cx, cy, cw, ch)
        applyWorkingBuffer()
    }

    // Divide image size by 2 with a bilinear 2x2 filter
    public func toCrop(using self, x, y, w, h: s32)->Image
    {
        cx := math.clamp(x, 0, width - 1)
        cy := math.clamp(y, 0, height - 1)
        cw := math.min(w, width - cx)
        ch := math.min(h, height - cy)
        debug.assert(cw > 0 and ch > 0)

        var result: retval = create(cw, ch, pf)
        doCrop(&result, cx, cy, cw, ch)
        return result
    }
}