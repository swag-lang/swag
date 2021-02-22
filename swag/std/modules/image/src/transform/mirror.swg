using core

impl Image
{
    func(T) mirror(using self)
    {
        ptrRowSrc := cast(*T) pixels
        ptrRowDst := cast(*T) pixels + width - 1
        loop x: width / 2
        {
            ptrSrc := ptrRowSrc
            ptrDst := ptrRowDst
            loop y: height
            {
                swap(ptrSrc, ptrDst)
                ptrSrc, ptrDst += width
            }

            ptrRowSrc += 1
            ptrRowDst -= 1
        }
    }

    // Flip image horizontally
    public func mirror(using self)
    {
        if !pixels return
        debug.assert(width && height)
        switch bpp
        {
        case 32:
            mirror'u32()
        }
    }
}