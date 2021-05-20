
using core

//#[Swag.printbc]
#test
{
    str := "日本語日本語日本語"

    c16 := utf16.fromUtf8(str)
    c8 := utf8.fromUtf16(c16.toSlice())
    @assert(c8.length == @countof(str))
    @assert(@memcmp(@dataof(str), c8.buffer, c8.length) == 0)
}