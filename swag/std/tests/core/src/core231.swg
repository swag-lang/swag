#import "core"
using core

//#[swag.printbc]
#test
{
    str := "日本語日本語日本語"

    c16 := unicode.fromUtf8(str)
    c8 := utf8.fromUnicode(c16.toSlice())
    @assert(c8.count == @countof(str))
    @assert(@memcmp(@dataof(str), c8.buffer, c8.count) == 0)
}