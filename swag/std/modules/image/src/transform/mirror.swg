using core

impl Image
{
    func(N: u8) mirror(using self)
    {
        ptrRowSrc := pixels
        ptrRowDst := pixels + (width - 1) * N
        loop width / 2
        {
            ptrSrc := ptrRowSrc
            ptrDst := ptrRowDst
            loop height
            {
                #if N == 1
                    swap(ptrSrc, ptrDst)
                #elif N == 2 || N == 3
                    swap(cast(*u16) ptrSrc, cast(*u16) ptrDst)
                #elif N == 3
                    swap(&ptrSrc[2], &ptrDst[2])
                #elif N == 4
                    swap(cast(*u32) ptrSrc, cast(*u32) ptrDst)
                #elif N == 8
                    swap(cast(*u64) ptrSrc, cast(*u64) ptrDst)

                ptrSrc, ptrDst += width * N
            }

            ptrRowSrc += N
            ptrRowDst -= N
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
            mirror'1()
        case 16:
            mirror'2()
        case 24:
            mirror'3()
        case 32:
            mirror'4()
        case 64:
            mirror'8()
        default:
            debug.assert(false, "unsupported pixel format")
        }
    }
}