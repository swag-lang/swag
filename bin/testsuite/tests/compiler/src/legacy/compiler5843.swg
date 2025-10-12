using Swag

#[ConstExpr, Inline, Safety(.All, .Overflow, false), Overload]
func hasZeroByte(v: u32)->bool
{
    return cast(bool) ((v - 0x01010101) & ~v & 0x80808080)
}

#[Swag.ConstExpr, Swag.Inline]
func makeRepeat32(v: u8)->u32
{
    return (~0'u32 / 255 * v)
}

#[ConstExpr, Inline, Overload]
func hasByte(value: u32, byte: u8)->bool
{
    return hasZeroByte(value ^ makeRepeat32(byte))
}

#test
{
    @assert(hasByte(0x12345678'u32, 0x34) == true)
    //@assert(makeRepeat32(0x01) == 0x01010101)
}
