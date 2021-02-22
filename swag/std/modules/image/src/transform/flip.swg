using core

impl Image
{
    // Flip image vertically
    public func flip(using self)
    {
        w := cast(uint) width * bpp8
        tmpLine := memory.alloc(w, @getcontext().tempAllocator)

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