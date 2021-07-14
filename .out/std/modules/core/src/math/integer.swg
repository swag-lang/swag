#global public
#global namespace Math
using Swag

#[ConstExpr]
func roundUpToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while i < x
        i <<= 1
    return i
}

#[ConstExpr]
func roundDownToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while (i << 1) <= x
        i <<= 1
    return i
}

#[ConstExpr, inline]
func isPowerOf2(x: u32)->bool
{
    Debug.assert(x != 0)
    return x & (x - 1) == 0
}

#[ConstExpr, inline]
{
    func sign(x: s8) => x < 0 ? -1 : 1
    func sign(x: s16) => x < 0 ? -1 : 1
    func sign(x: s32) => x < 0 ? -1 : 1
    func sign(x: s64) => x < 0 ? -1 : 1
}

#[ConstExpr, inline]
{
    func min(x, y: s8)  => @min(x, y)
    func min(x, y: s16) => @min(x, y)
    func min(x, y: s32) => @min(x, y)
    func min(x, y: s64) => @min(x, y)

    func max(x, y: s8)  => @max(x, y)
    func max(x, y: s16) => @max(x, y)
    func max(x, y: s32) => @max(x, y)
    func max(x, y: s64) => @max(x, y)

    func min(x, y: u8)  => @min(x, y)
    func min(x, y: u16) => @min(x, y)
    func min(x, y: u32) => @min(x, y)
    func min(x, y: u64) => @min(x, y)

    func max(x, y: u8)  => @max(x, y)
    func max(x, y: u16) => @max(x, y)
    func max(x, y: u32) => @max(x, y)
    func max(x, y: u64) => @max(x, y)

    func abs(x: s8)  => @abs(x)
    func abs(x: s16) => @abs(x)
    func abs(x: s32) => @abs(x)
    func abs(x: s64) => @abs(x)
}
