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
                idx := height - y - 1
                if idx <= y break
                tmp := getPixel(x, y)
                setPixel(x, y, getPixel(x, idx))
                setPixel(x, idx, tmp)
            }
        }
    }
}