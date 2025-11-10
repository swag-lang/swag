#[Swag.Optimize(.All, false)]
func(T) toto(x: T)->T
{
    return @bitcountnz(x)
}

#test
{
    let a0 = toto'u8(0x04)
    @assert(a0 == 1)
    let a1 = toto'u16(0x0401)
    @assert(a1 == 2)
    let a2 = toto'u32(0x04000000)
    @assert(a2 == 1)
    let a3 = toto'u64(0x00800000_00000011)
    @assert(a3 == 3)
}
