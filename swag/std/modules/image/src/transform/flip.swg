using core

public func flip(src: Image)->Image
{
    var result: retval = create(src.width, src.height, src.pf)

    w := cast(uint) src.width * src.bpp8
    ptrDst := result.pixels + w * (src.height - 1)
    ptrSrc := src.pixels

    loop h: src.height
    {
        memory.copy(ptrDst, ptrSrc, w)
        ptrSrc += w
        ptrDst -= w
    }

    return result
}