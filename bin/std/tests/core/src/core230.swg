using Core

//#[Swag.PrintBc]
#test
{
    let str = "日本語日本語日本語"

    var c16 = Utf16.fromUtf8(str)
    let c8  = Utf8.fromUtf16(c16.toSlice())
    @assert(c8.length == @countof(str))
    @assert(@memcmp(@dataof(str), c8.buffer, c8.length) == 0)
}
