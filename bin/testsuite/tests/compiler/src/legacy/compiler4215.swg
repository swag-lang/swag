#[Swag.Overload]
func(T) toto(y: T)->T
{
    var x = y
    x %= 3
    return x
}

#[Swag.Overload]
func(T) toto(y, z: T)->T
{
    var x1 = y
    var x2 = z
    x2 += 1
    x2 -= 1
    x1 %= x2
    return x1
}

#test
{
    @assert(toto'u8(6) == 0)
    @assert(toto'u8(5) == 2)
    /*@assert(toto'u8(6, 3) == 0)
    @assert(toto'u8(5, 3) == 2)

    @assert(toto'u16(6) == 0)
    @assert(toto'u16(5) == 2)
    @assert(toto'u16(6, 3) == 0)
    @assert(toto'u16(5, 3) == 2)

    @assert(toto'u32(6) == 0)
    @assert(toto'u32(5) == 2)
    @assert(toto'u32(6, 3) == 0)
    @assert(toto'u32(5, 3) == 2)

    @assert(toto'u64(6) == 0)
    @assert(toto'u64(5) == 2)
    @assert(toto'u64(6, 3) == 0)
    @assert(toto'u64(5, 3) == 2)*/
}
