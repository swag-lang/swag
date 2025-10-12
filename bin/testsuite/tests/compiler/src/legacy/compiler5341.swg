#[Swag.Optimize(.All, false)]
func(T) toto(x: T)->T
{
    return @byteswap(x)
}

#test
{
    let a0 = toto'u16(0x0102)
    @assert(a0 == 0x0201)
    let a1 = toto'u32(0x01010202)
    @assert(a1 == 0x02020101)
    let a2 = toto'u64(0x01010101_02020202)
    @assert(a2 == 0x02020202_01010101)
}
