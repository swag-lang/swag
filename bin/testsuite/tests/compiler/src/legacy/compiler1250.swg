func getChar(buffer: const [*] u8)->{ a: const [*] u8, b: rune }
{
    var ptr = buffer
    var c   = cast(rune) dref ptr
    if c == 0 do
        return {ptr, cast() 0}

    ptr += 1
    if ((c & 0x80) == 0)
    {
        return {ptr, cast() c}
    }

    var wc: rune = undefined
    if ((c & 0xE0) == 0xC0)
    {
        wc = (c & 0x1F) << 6; c  = cast(rune) dref ptr
        ptr += 1
        wc |= (c & 0x3F); return {ptr, wc}
    }

    if ((c & 0xF0) == 0xE0)
    {
        wc = (c & 0xF) << 12; wc |= cast(rune) (ptr[0] & 0x3F) << 6; wc |= cast(rune) (ptr[1] & 0x3F); ptr += 2; return {ptr, wc}
    }

    if ((c & 0xF8) == 0xF0)
    {
        wc = (c & 0x7) << 18; wc |= cast(rune) (ptr[0] & 0x3F) << 12; wc |= cast(rune) (ptr[1] & 0x3F) << 6; wc |= cast(rune) (ptr[2] & 0x3F); ptr += 3; return {ptr, wc}
    }

    if ((c & 0xFC) == 0xF8)
    {
        wc = (c & 0x3) << 24; wc |= (c & 0x3F) << 18; wc |= (c & 0x3F) << 12; wc |= (c & 0x3F) << 6; wc |= (c & 0x3F); return {ptr, wc}
    }

    if ((c & 0xFE) == 0xFC)
    {
        wc = (c & 0x1) << 30; wc |= (c & 0x3F) << 24; wc |= (c & 0x3F) << 18; wc |= (c & 0x3F) << 12; wc |= (c & 0x3F) << 6; wc |= (c & 0x3F); return {ptr, wc}
    }

    return {ptr, cast() 0}
}

#test
{
    const str = "⺁⺆⻇"

    var c:   rune
    var ptr: const [*] u8 = @dataof(str)
    var cpt  = 0
    for
    {
        (ptr, c) = getChar(ptr)
        if !c do
            break
        cpt += 1
    }

    @assert(#sizeof(str) != 3)
    @assert(cpt == 3)
}
