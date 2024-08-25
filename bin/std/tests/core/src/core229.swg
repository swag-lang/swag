using Core.Utf16

var t: [] struct
{
    inT:      const [..] rune
    outT:     const [..] u16
}
 = [
    {[1, 2, 3, 4], [1, 2, 3, 4]},
    {[0xffff, 0x10000, 0x10001, 0x12345, 0x10ffff], [0xffff, 0xd800, 0xdc00, 0xd800, 0xdc01, 0xd808, 0xdf45, 0xdbff, 0xdfff]},
    {[`a`'rune, `b`'rune, 0xd7ff, 0xd800, 0xdfff, 0xe000, 0x110000, 0xffffffff], [`a`'u16, `b`'u16, 0xd7ff, 0xfffd, 0xfffd, 0xe000, 0xfffd, 0xfffd]}]

//#[Swag.PrintBc]
#test
{
    foreach &p in t
    {
        var r = fromUnicode(p.inT)
        @assert(r.count == @countof(p.outT))
        @assert(@memcmp(r.buffer, p.outT, @countof(p.outT) * 2) == 0)
    }
}
