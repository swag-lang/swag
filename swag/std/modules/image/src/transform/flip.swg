using core

impl Image
{
    // Flip image vertically
    public func flip(using self)
    {
        if !pixels return
        debug.assert(width && height)

        w := cast(uint) width * bpp8
        tmpLine := memory.tempAlloc(w)

        ptrDst := pixels + w * (height - 1)
        ptrSrc := pixels

        loop h: height
        {
            if ptrSrc == ptrDst break
            memory.copy(tmpLine, ptrDst, w)
            memory.copy(ptrDst, ptrSrc, w)
            memory.copy(ptrSrc, tmpLine, w)
            ptrSrc += w
            ptrDst -= w
        }
    }
}