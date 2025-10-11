func decode(destBuffer: [*] void, destLength: u64, srcBuffer: [*] void, srcLength: u64)->{ write, read: u64 } throw
{
    func(TD, TS) doIt(destBuffer: [*] void, destLength: u64, srcBuffer: [*] void, srcLength: u64)->{ write, read: u64 }
    {
        var dst = cast([*] TD) destBuffer
        var src = cast([*] TS) srcBuffer

        var write = 0
        for i in srcLength
        {
            if write + #sizeof(TD) > destLength do
                break
            dst[0] = (cast(TD) src[0] - 128) << 8
            dst += 1
            src += 1
            write += #sizeof(TD)
        }

        return {write, write / 2}
    }

    return doIt'(s16, s8)(destBuffer, destLength, srcBuffer, srcLength)
}
