#[Swag.Overload]
func(T) toto(y: T)->T
{
    var x = y
    x /= 3
    return x
}

#[Swag.Overload]
func(T) toto(y, z: T)->T
{
    var x1 = y
    var x2 = z
    x2 += 1
    x2 -= 1
    x1 /= x2
    return x1
}

#test
{
    @assert(toto'u8(6) == 2)
    @assert(toto'u8(9, 3) == 3)
    @assert(toto'u8(255, 2) == 127)

    @assert(toto'u16(6) == 2)
    @assert(toto'u16(9, 3) == 3)
    @assert(toto'u16(255, 2) == 127)
    @assert(toto'u16(0xFFFF, 2) == 0xFFFF >> 1)

    @assert(toto'u32(6) == 2)
    @assert(toto'u32(9, 3) == 3)
    @assert(toto'u32(255, 2) == 127)
    @assert(toto'u32(0xFFFF_FFFF, 2) == 0xFFFF_FFFF >> 1)

    @assert(toto'u64(6) == 2)
    @assert(toto'u64(9, 3) == 3)
    @assert(toto'u64(255, 2) == 127)
    @assert(toto'u64(0xFFFF_FFFF_FFFF_FFFF, 2) == 0xFFFF_FFFF_FFFF_FFFF >> 1)

    @assert(toto'f32(6) == 2)
    @assert(toto'f32(9, 3) == 3)
    @assert(toto'f32(254, 2) == 127)

    @assert(toto'f64(6) == 2)
    @assert(toto'f64(9, 3) == 3)
    @assert(toto'f64(254, 2) == 127)
}
