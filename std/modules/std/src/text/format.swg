namespace std.text
{
    using std.collections, std.diagnostics

    public func format(buf: *ConcatBuffer, fmt: string, values: ...)
    {
        ptr := @dataof(fmt)
        cpt := @countof(fmt)

        buf.clear()
        idx := 0'u32
        nextIdx := 0'u32
        paramIdx := 0'u32
        numParams := @countof(values)

        while nextIdx < cpt
        {
            if ptr[nextIdx] == cast(u8) "%"'char
            {
                // Add remaining text, and starts again after the mark
                buf.addBytes(cast([..] u8) {ptr + idx, nextIdx - idx})
                idx = nextIdx + 1
                nextIdx = idx

                // Deal with the parameter. %% is for one % only
                if ptr[nextIdx] == cast(u8) "%"'char
                {
                    buf.addByte(cast(u8) "%"'char)
                    nextIdx += 1
                    idx += 1
                    continue
                }  

                // Deal with the parameter
                debug.assert(paramIdx < numParams, "not enough 'format' parameters")
                convertAny(buf, values[paramIdx])
                paramIdx += 1
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
