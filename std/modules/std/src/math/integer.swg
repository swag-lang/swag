public namespace std.math;

#[swag.constexpr]
func bitIndex(x: u32)->u32
{
    i := 0'u32
    val := x
    while val
    {
        if val & 1
            return i
        val >>= 1
        i += 1
    }

    return i
}

#[swag.constexpr]
func roundUpToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while i < x
        i <<= 1
    return i
}

#[swag.constexpr]
func roundDownToPowerOf2(x: u32)->u32
{
    i := 1'u32
    while (i << 1) <= x
        i <<= 1
    return i
}

#[swag.constexpr, swag.inline]
{
    func sign(x: s8) => x < 0 ? -1 : 1
    func sign(x: s16) => x < 0 ? -1 : 1
    func sign(x: s32) => x < 0 ? -1 : 1
    func sign(x: s64) => x < 0 ? -1 : 1
}

#[swag.constexpr, swag.inline]
{
    func abs(x: s8) => @abs(x)
    func abs(x: s16) => @abs(x)
    func abs(x: s32) => @abs(x)
    func abs(x: s64) => @abs(x)
}
