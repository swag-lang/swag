#global public
#global namespace format
using swag

func checkFormat(fmt: string, values: ...)
{
    cptPercent := countPercent(fmt)
    if cptPercent != @countof(values)
    {
        #[selectif(false)]
        str := toString("bad number of arguments to 'format.append' ('%' requested, '%' provided)", cptPercent, @countof(values))
        @error(str, #location(values))
    }
}

// Count the number of valid '%' placeholders in the string format
// Note that %% will not count, as this represents the '%' character
func countPercent(fmt: string)->u32
{
    cptPercent := 0'u32
    for i := 0; i < @countof(fmt); i += 1
    {
        if fmt[i] != "%"'u8
            continue

        if i == @countof(fmt) - 1 || fmt[i + 1] != "%"'u8
            cptPercent += 1
        else
            i += 1
    }

    return cptPercent
}

// Format a string and returns the result as a String
func toString(fmt: string, values: ...)->String
    #selectif
    {
        if !@isconstexpr(fmt) return true
        checkFormat(fmt, values)
        return true
    }
{
    var buf: ConcatBuffer
    #[selectif(false)]
    append(&buf, fmt, values)
    return buf.moveToString()
}

// Format a string and put the result in a ConcatBuffer
func append(buf: *ConcatBuffer, fmt: string, values: ...)
    #selectif
    {
        if !@isconstexpr(fmt) return true
        checkFormat(fmt, values)
        return true
    }
{
    ptr := @dataof(fmt)
    cpt := @countof(fmt)

    idx := 0'u32
    nextIdx := 0'u32
    paramIdx := 0'u32
    numParams := @countof(values)

    while nextIdx < cpt
    {
        if ptr[nextIdx] == "%"'u8
        {
            // Add remaining text, and starts again after the mark
            buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, cast(uint) (nextIdx - idx)))
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
                strFormat = @mkslice(ptrFormat, cast(uint) countFormat)
            }

            // Deal with the parameter
            debug.assert(paramIdx < numParams, "not enough 'format' parameters")
            strconv.convertAny(buf, values[paramIdx], cast(string) strFormat)
            paramIdx += 1
        }
        else
        {
            nextIdx += 1
        }
    }

    // Add remaining text
    if idx < nextIdx
        buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, cast(uint) nextIdx - idx))
}
