/*
    :LICENCE
    This code is based on the Go library (https://github.com/golang/go/blob/master/src/math/bits/bits.go)
    The original code has been modified for Swag
    See LICENCE.md for the corresponding licence.
*/

#global public
#global namespace Math
using Swag

#[Swag.CanOverflow(true)]
func mulU64(x, y: u64)->{ hi: u64, lo: u64 }
{
    const mask32 = (1'u64 << 32) - 1
    let x0       = x & mask32
    let x1       = x >> 32
    let y0       = y & mask32
    let y1       = y >> 32
    let w0       = x0 * y0
    let t        = (x1 * y0) + (w0 >> 32)
    var w1       = t & mask32
    let w2       = t >> 32
    w1 += x0 * y1

    var result: retval
    result.hi = (x1 * y1) + w2 + (w1 >> 32)
    result.lo = x * y
    return result
}

struct Int128
{
    lo:     u64
    hi:     s64
}

impl Int128
{
    mtd const opCmp(other: Int128)->s32
    {
        if .hi != other.hi do
            return .hi <=> other.hi
        return .lo <=> other.lo
    }

    mtd const opCast()->f64
    {
        const Shift64: f64 = cast #bit (f64) 0x43f0000000000000 // 18446744073709551616.0'f64 //2^64
        if .hi < 0
        {
            if .lo == 0 do
                return cast(f64) (.hi * Shift64)
            else do
                return -cast(f64) (~.lo + ~.hi * Shift64)
        }

        return cast(f64) (.lo + .hi * Shift64)
    }

    func mul(x, y: s64)->Int128
    {
        var lhs = x
        var rhs = y

        var result: retval

        let negate = (lhs < 0) != (rhs < 0)

        if lhs < 0 do
            lhs = -lhs
        let int1Hi = cast(u64) lhs >> 32
        let int1Lo = cast(u64) (lhs & 0xFFFFFFFF)

        if rhs < 0 do
            rhs = -rhs
        let int2Hi = cast(u64) rhs >> 32
        let int2Lo = cast(u64) (rhs & 0xFFFFFFFF)

        let a = int1Hi * int2Hi
        let b = int1Lo * int2Lo
        let c = int1Hi * int2Lo + int1Lo * int2Hi

        result.hi = cast(s64) (a + (c >> 32))
        result.lo = cast(s64) (c << 32)
        result.lo += cast(s64) b
        if result.lo < b do
            result.hi += 1

        if negate
        {
            if result.lo == 0 do
                result.hi = -result.hi
            else
            {
                result.hi = ~result.hi
                result.lo = ~result.lo + 1
            }
        }

        return result
    }
}
