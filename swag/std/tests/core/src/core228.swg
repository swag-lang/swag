#import "core"
using core.utf16

var t: [] struct {
    r: [2] u16
    w: char
} = @[
    {[0xd800, 0xdc00], 0x10000},
    {[0xd800, 0xdc01], 0x10001},
    {[0xd808, 0xdf45], 0x12345},
    {[0xdbff, 0xdfff], 0x10ffff},
    {[0xd800, "a"'u16], 0xfffd},
]

#test
{
    visit *p: t
    {
        res := decodeChar(p.r)
        @assert(res.c == p.w)
    }
}