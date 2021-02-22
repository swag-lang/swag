using core

impl Image
{
    func mirror24(using self)
    {
        ptrRowSrc := pixels
        ptrRowDst := pixels + (width - 1) * 3
        loop x: width / 2
        {
            ptrSrc := ptrRowSrc
            ptrDst := ptrRowDst
            loop y: height
            {
                swap(cast(*u16) ptrSrc, cast(*u16) ptrDst)
                swap(&ptrSrc[2], &ptrDst[2])
                ptrSrc, ptrDst += width * 3
            }

            ptrRowSrc += 3
            ptrRowDst -= 3
        }
    }

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
        case 8:
            mirror'u8()
        case 16:
            mirror'u16()
        case 24:
            mirror24()
        case 32:
            mirror'u32()
        case 64:
            mirror'u64()
        default:
            debug.assert(false, "unsupported pixel format")
        }
    }
}