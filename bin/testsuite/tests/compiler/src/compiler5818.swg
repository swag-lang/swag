func(T) totoRol(a: T, b: u8)->T
{
    return @rol(a, b)
}

func(T) totoRor(a: T, b: u8)->T
{
    return @ror(a, b)
}

#[Swag.Optimize(.All, false)]
//#[Swag.PrintBc]
#test
{
    @assert(totoRol(1'u8, 2) == 4)
    @assert(totoRol(128'u8, 1) == 1)
    @assert(totoRol(1'u16, 2) == 4)
    @assert(totoRol(0x8000'u16, 2) == 2)
    @assert(totoRol(0x80000000'u32, 2) == 2)
    @assert(totoRol(0x80000000_00000000'u64, 2) == 2)

    @assert(totoRor(1'u8, 2) == 64)
    @assert(totoRor(1'u16, 2) == 0x4000)
    @assert(totoRor(1'u32, 2) == 0x40000000)
    @assert(totoRor(1'u64, 2) == 0x40000000_00000000)
}
