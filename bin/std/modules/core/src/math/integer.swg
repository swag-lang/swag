// Some magical integer tricks comes from https://graphics.stanford.edu/~seander/bithacks.html#ValueInWord

#global public
#global namespace Math
using Swag

#[ConstExpr, Safety(.All, .Overflow, false), Overload]
{
    func roundUpToPowerOf2(x: u32)->u32   => 1'u32 << 32 - Math.leadingZeros(x)
    func roundUpToPowerOf2(x: u64)->u64   => 1'u64 << cast(u32) (64 - Math.leadingZeros(x))
    func roundDownToPowerOf2(x: u32)->u32 => 1'u32 << (31 - Math.leadingZeros(x))
    func roundDownToPowerOf2(x: u64)->u64 => 1'u64 << cast(u32) (63 - Math.leadingZeros(x))
    func isPowerOf2(x: u8)->bool          => x and !(x & (x - 1))
    func isPowerOf2(x: u16)->bool         => x and !(x & (x - 1))
    func isPowerOf2(x: u32)->bool         => x and !(x & (x - 1))
    func isPowerOf2(x: u64)->bool         => x and !(x & (x - 1))
}

// Determin if any of the bytes is zero
#[ConstExpr, Inline, Safety(.All, .Overflow, false), Overload]
func hasZeroByte(v: u16)->bool
{
    return hasZeroByte(cast(u32) v)
}

#[ConstExpr, Inline, Safety(.All, .Overflow, false), Overload]
func hasZeroByte(v: u32)->bool
{
    return cast(bool) ((v - 0x01010101) & ~v & 0x80808080)
}

#[ConstExpr, Inline, Safety(.All, .Overflow, false), Overload]
func hasZeroByte(v: u64)->bool
{
    return cast(bool) ((v - 0x01010101_01010101) & ~v & 0x80808080_80808080)
}

// Make a 16 bits by repeating a given byte
// 0x20 => 0x2020
#[ConstExpr, Inline]
func makeRepeat16(v: u8)->u16
{
    return (~0'u16 / 255 * v)
}

// Make a 32 bits by repeating a given byte
// 0x20 => 0x20202020
#[ConstExpr, Inline]
func makeRepeat32(v: u8)->u32
{
    return (~0'u32 / 255 * v)
}

// Make a 64 bits by repeating a given byte
// 0x20 => 0x20202020_20202020
#[ConstExpr, Inline]
func makeRepeat64(v: u8)->u64
{
    return (~0'u64 / 255 * v)
}

// Make a 64 bits with two 32 bits
#[ConstExpr, Inline]
func make64(high, low: u32)->u64
{
    return (cast(u64) high << 32) | low
}

// Determin if an integer has a given byte
#[ConstExpr, Inline, Overload]
func hasByte(value: u16, byte: u8)->bool
{
    return hasByte(cast(u32) value, byte)
}

#[ConstExpr, Inline, Overload]
func hasByte(value: u32, byte: u8)->bool
{
    return hasZeroByte(value ^ makeRepeat32(byte))
}

#[ConstExpr, Inline, Overload]
func hasByte(value: u64, byte: u8)->bool
{
    return hasZeroByte(value ^ makeRepeat64(byte))
}

// Get the sign of an integer.
// Will return -1, 1 or 0 for a zero value.
#[ConstExpr, Inline, Overload]
{
    func sign(x: s8)  => cast(s8) (x > 0) - cast(s8) (x < 0)
    func sign(x: s16) => cast(s16) (x > 0) - cast(s16) (x < 0)
    func sign(x: s32) => cast(s32) (x > 0) - cast(s32) (x < 0)
    func sign(x: s64) => cast(s64) (x > 0) - cast(s64) (x < 0)
}

// Wrappers for intrinsics
#[ConstExpr, Inline, Overload]
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

// Find the gcd between 'a' and 'b'
#[ConstExpr]
func gcd(a0, b0: s64)->s64
{
    var a = a0
    var b = b0
    while (b != 0)
    {
        let temp = b
        b = a % b
        a = temp
    }

    return a
}

// Find the Least Common Multiple between 'a' and 'b'
#[ConstExpr]
func lcm(a, b: s64)->s64
{
    if a > b do
        return a / gcd(a, b) * b
    else do
        return b / gcd(a, b) * a
}
