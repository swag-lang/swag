#global public
#global namespace math
using swag

#[constexpr]
func roundUpToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while i < x
        i <<= 1
    return i
}

#[constexpr]
func roundDownToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while (i << 1) <= x
        i <<= 1
    return i
}

#[constexpr, inline]
func isPowerOf2(x: u32)->bool
{
    debug.assert(x != 0)
    return x & (x - 1) == 0
}

#[constexpr, swag.inline]
{
    func sign(x: s8) => x < 0 ? -1 : 1
    func sign(x: s16) => x < 0 ? -1 : 1
    func sign(x: s32) => x < 0 ? -1 : 1
    func sign(x: s64) => x < 0 ? -1 : 1
}

#[constexpr, swag.inline]
{
    func abs(x: s8) => @abs(x)
    func abs(x: s16) => @abs(x)
    func abs(x: s32) => @abs(x)
    func abs(x: s64) => @abs(x)
}
