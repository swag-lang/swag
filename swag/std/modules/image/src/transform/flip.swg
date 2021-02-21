using core

impl Image
{
    // Flip image vertically
    public func getFlip(using self)->Image
    {
        var result: retval = create(width, height, pf)

        w := cast(uint) width * bpp8
        ptrDst := result.pixels + w * (height - 1)
        ptrSrc := pixels

        loop h: height
        {
            memory.copy(ptrDst, ptrSrc, w)
            ptrSrc += w
            ptrDst -= w
        }

        return result
    }

    // Flip image vertically
    public func flip(using self)
    {
        loop x: width
        {
            loop y: height
            {
                idx := height - cast(u32) y - 1
                if idx <= y break
                tmp := getPixel(cast(u32) x, cast(u32) y)
                setPixel(cast(u32) x, cast(u32) y, getPixel(cast(u32) x, idx))
                setPixel(cast(u32) x, idx, tmp)
            }
        }
    }
}