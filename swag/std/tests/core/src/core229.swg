#import "core"
using core.utf16

var t: [] struct {
    in:  [..] char
    out: [..] u16
} = @[
    {[1,2,3,4], [1,2,3,4]},
    {[0xffff, 0x10000, 0x10001, 0x12345, 0x10ffff], [0xffff, 0xd800, 0xdc00, 0xd800, 0xdc01, 0xd808, 0xdf45, 0xdbff, 0xdfff]},
    {["a"'char, "b"'char, 0xd7ff, 0xd800, 0xdfff, 0xe000, 0x110000, -1], ["a"'u16, "b"'u16, 0xd7ff, 0xfffd, 0xfffd, 0xe000, 0xfffd, 0xfffd]},
]

//#[swag.printbc]
#test

{
    visit *p: t
    {
        r := fromCharArray(p.in)
        @assert(r.count == @countof(p.out))
        @assert(@memcmp(r.buffer, p.out, @countof(p.out) * 2) == 0)
    }
}