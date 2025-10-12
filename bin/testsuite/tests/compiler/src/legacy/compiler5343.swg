#[Swag.Optimize(.All, false)]
func(T) toto(x: T)->T
{
    return @bitcountlz(x)
}

#test
{
    let a0 = toto'u8(0x04)
    @assert(a0 == 5)
    let a1 = toto'u16(0x0400)
    @assert(a1 == 5)
    let a2 = toto'u32(0x04000000)
    @assert(a2 == 5)
    let a3 = toto'u64(0x00800000_00000000)
    @assert(a3 == 8)
}
