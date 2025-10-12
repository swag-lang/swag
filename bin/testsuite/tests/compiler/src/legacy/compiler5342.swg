#[Swag.Optimize(.All, false)]
func(T) toto(x: T)->T
{
    return @bitcounttz(x)
}

#test
{
    let a0 = toto'u16(0x80)
    @assert(a0 == 7)
    let a1 = toto'u16(0x0180)
    @assert(a1 == 7)
    let a2 = toto'u32(0x0180)
    @assert(a2 == 7)
    let a3 = toto'u64(0x01800)
    @assert(a3 == 11)
}
