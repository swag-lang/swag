namespace std.text
{
    using std.collections

    // Format a string and returns the result as a String
    public func format(fmt: string, values: ...)->String
    {
        var buf: ConcatBuffer
        format(&buf, fmt, values)
        return buf.toString()
    }

    // Format a string and put the result in a ConcatBuffer
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
            if ptr[nextIdx] == "%"'u8
            {
                // Add remaining text, and starts again after the mark
                buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, nextIdx - idx))
                idx = nextIdx + 1
                nextIdx = idx

                // Deal with the parameter. %% is for one % only
                if ptr[nextIdx] == "%"'u8
                {
                    buf.addByte("%"'u8)
                    nextIdx += 1
                    idx += 1
                    continue
                }

                // Optional string format, like '%{whatever}'
                var strFormat: const [..] u8
                if ptr[nextIdx] == "{"'u8
                {
                    nextIdx += 1
                    ptrFormat := ptr + nextIdx
                    countFormat := 0'u32
                    while ptr[nextIdx] && ptr[nextIdx] != "}"'u8
                    {
                        countFormat += 1
                        nextIdx += 1
                    }

                    if ptr[nextIdx]
                        nextIdx += 1
                    idx = nextIdx
                    strFormat = @mkslice(ptrFormat, countFormat)
                }

                // Deal with the parameter
                debug.assert(paramIdx < numParams, "not enough 'format' parameters")
                convertAny(buf, values[paramIdx], cast(string) strFormat)
                paramIdx += 1
            }
            else
            {
                nextIdx += 1
            }
        }

        // Add remaining text
        if idx < nextIdx
            buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, nextIdx - idx))
    }
}
