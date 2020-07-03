namespace std.text
{
    using std.collections

    public func format(buf: *ConcatBuffer, fmt: string, values: ...)
    {
        ptr := @dataof(fmt)
        cpt := @countof(fmt)

        idx := 0'u32
        nextIdx := 0'u32
        paramIdx := 0'u32

        while nextIdx < cpt
        {
            if ptr[nextIdx] == cast(u8) "%"'char
            {
                if ptr[nextIdx + 1] == cast(u8) "%"'char
                {
                    buf.addByte(cast(u8) "%"'char)
                    continue
                }

                // Add remaining text, and starts again after the mark
                buf.addBytes(cast([..] u8) {ptr + idx, nextIdx - idx})
                idx = nextIdx + 1
                nextIdx = idx

                // Deal with the parameter
            }
            else
            {
                nextIdx += 1
            }
        }

        // Add remaining text
        if idx < nextIdx
            buf.addBytes(cast([..] u8) {ptr + idx, nextIdx - idx})
    }
}
