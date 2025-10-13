#global public
#global namespace Format
using Swag

// Check if this is a valid 'fmt/values' pair, coherent with 'format' specification
func checkFormat(fct: string, fmt: string, values: ...)
{
    let cptPercent = countPercent(fmt)
    if cptPercent != @countof(values)
    {
        #[Match(.Where, false)]
        let str = toString("bad number of arguments to [[%]] ([[%]] requested, [[%]] provided)", fct, cptPercent, @countof(values))
        @compilererror(str, #location(values))
    }
}

// Count the number of valid '%' placeholders in the string format.
// Note that '%%' will not be counted, as this represents the '%' character
#[ConstExpr]
func countPercent(fmt: string)->u32
{
    var cptPercent = 0'u32
    for var i = 0; i < @countof(fmt); i += 1
    {
        if fmt[i] != '%''u8 do
            continue

        if i == @countof(fmt) - 1 or fmt[i + 1] != '%''u8 do
            cptPercent += 1
        else do
            i += 1
    }

    return cptPercent
}

// Format a string in 'dest'.
// Can be a lot faster than the convenient version which returns a String.
func replaceString(dest: *String, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt) or !#isconstexpr(values) do
            return true
        checkFormat(#curlocation.funcName, fmt, values)
        return true
    }
{
    var buf: ConcatBuffer

    buf.allocator = dest.allocator
    buf.setFirstBucket(@mkslice(dest.buffer, dest.capacity))

    #[Match(.Where, false)]
    append(&buf, fmt, values)

    buf.makeLinear()
    let (data, count, capacity) = buf.eatBuffer()

    dest.buffer   = data
    dest.length   = count
    dest.capacity = capacity
}

// Format a string and returns the result as a String.
// Use '%' as a place holder for a value.
// ```swag
// let x = Format.toString("my life is %", myLife)
// ```
// '%' can be followed by a format option between '{}'.
// ```swag
// let x = Format.toString("my life in hexadecimal is %{X}", myLife)
// ```
// * See [[ConvertFormatInt.setFormat]] for some valid format options
// * See [[ConvertFormatFloat.setFormat]] for some valid format options
func toString(fmt: string, values: ...)->String
    verify
    {
        if !#isconstexpr(fmt) or !#isconstexpr(values) do
            return true
        checkFormat(#curlocation.funcName, fmt, values)
        return true
    }
{
    var buf: ConcatBuffer

    #[Match(.Where, false)]
    append(&buf, fmt, values)

    return buf.toString()
}

// Returns a formatted string stored in the temporary allocator.
func toStringTemp(fmt: string, values: ...)->string
    verify
    {
        if !#isconstexpr(fmt) or !#isconstexpr(values) do
            return true
        checkFormat(#curlocation.funcName, fmt, values)
        return true
    }
{
    var buf: ConcatBuffer

    #[Match(.Where, false)]
    append(&buf, fmt, values)

    let str = buf.toString(@getcontext().tempAllocator)
    return str.toString()
}

// Format a string and put the result in a ConcatBuffer.
func append(buf: *ConcatBuffer, fmt: string, values: ...)
    verify
    {
        if !#isconstexpr(fmt) or !#isconstexpr(values) do
            return true
        checkFormat(#curlocation.funcName, fmt, values)
        return true
    }
{
    let ptr = @dataof(fmt)
    let cpt = @countof(fmt)

    var idx       = 0'u32
    var nextIdx   = 0'u32
    var paramIdx  = 0'u32
    let numParams = @countof(values)

    let convFmt: StrConv.ConvertFormat

    while nextIdx < cpt
    {
        if ptr[nextIdx] == '%''u8
        {
            // Add remaining text, and starts again after the mark
            buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, cast(u64) (nextIdx - idx)))
            idx     = nextIdx + 1
            nextIdx = idx

            // Deal with the parameter. %% is for one % only
            if ptr[nextIdx] == '%''u8
            {
                buf.addNative('%''u8)
                nextIdx += 1
                idx += 1
                continue
            }

            // Optional string format, like '%{whatever}'
            var strFormat: const [..] u8
            if ptr[nextIdx] == '{''u8
            {
                nextIdx += 1
                let ptrFormat   = ptr + nextIdx
                var countFormat = 0'u32
                while ptr[nextIdx] and ptr[nextIdx] != '}''u8
                {
                    countFormat += 1
                    nextIdx += 1
                }

                if ptr[nextIdx] do
                    nextIdx += 1
                idx       = nextIdx
                strFormat = @mkslice(ptrFormat, cast(u64) countFormat)
            }

            // Deal with the parameter
            Debug.assert(paramIdx < numParams, "not enough 'format' parameters")
            StrConv.convertAny(buf, values[paramIdx], convFmt, cast(string) strFormat)
            paramIdx += 1
        }
        else
        {
            nextIdx += 1
        }
    }

    // Add remaining text
    if idx < nextIdx do
        buf.addBytes(cast(const [..] u8) @mkslice(ptr + idx, cast(u64) nextIdx - idx))
}

// String interpolation.
// Format is `{<value to interpolate>:format}`
#[Macro, Overload]
func toInterp(buf: *ConcatBuffer, exp: string)
    verify
    {
        if !#isconstexpr(exp) do
            @compilererror("expression should be constant", #location(exp))
        return true
    }
{
    #ast
    {
        var sb: StrConv.StringBuilder
        sb.appendString("var convFmt: StrConv.ConvertFormat\n")

        var idx = 0'u64
        while idx != @countof(exp)
        {
            // To the interpolated string if it exists
            var idxE = Utf8.indexOf(exp, "{", startByteIndex: idx)
            if idxE == Swag.U64.Max
            {
                sb.appendFormat("buf.addBytes(\"%\")\n", exp[idx to ])
                break
            }

            if idx != idxE do
                sb.appendFormat("buf.addBytes(\"%\")\n", exp[idx until idxE])

            // Find the end of the interpolated string, or the start of the format
            idx  = idxE + 1
            idxE = Utf8.indexOfAny(exp, "}:", startByteIndex: idx)
            if idxE == Swag.U64.Max
            {
                sb.appendFormat("buf.addBytes(\"%\")\n", exp[idx - 1 to ])
                break
            }

            let value = exp[idx until idxE]
            idx = idxE + 1

            // Format
            var format: string
            if exp[idxE] == ':''u8
            {
                idxE = Utf8.indexOf(exp, "}", startByteIndex: idx)
                if idxE != Swag.U64.Max
                {
                    format = exp[idx until idxE]
                    idx    = idxE + 1
                }
            }

            sb.appendFormat("StrConv.convert(buf, #up %, convFmt, \"%\")\n", value, format)
        }

        return sb.moveToString()
    }
}

// String interpolation.
#[Mixin, Overload]
func toInterp(exp: string)->String
    verify
    {
        if !#isconstexpr(exp) do
            @compilererror("expression should be constant", #location(exp))
        return true
    }
{
    var #uniq0: ConcatBuffer
    toInterp(&#uniq0, exp)
    return #uniq0.moveToString()
}
