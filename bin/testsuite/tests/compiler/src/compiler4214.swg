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
    @assert(toto's8(6) == 0)
    @assert(toto's8(5) == 2)
    @assert(toto's8(-5) == -2)
    @assert(toto's8(6, 3) == 0)
    @assert(toto's8(5, 3) == 2)
    @assert(toto's8(-5, 3) == -2)
    @assert(toto's8(-5, -3) == -2)

    @assert(toto's16(6) == 0)
    @assert(toto's16(5) == 2)
    @assert(toto's16(-5) == -2)
    @assert(toto's16(6, 3) == 0)
    @assert(toto's16(5, 3) == 2)
    @assert(toto's16(-5, 3) == -2)
    @assert(toto's16(-5, -3) == -2)

    @assert(toto's32(6) == 0)
    @assert(toto's32(5) == 2)
    @assert(toto's32(-5) == -2)
    @assert(toto's32(6, 3) == 0)
    @assert(toto's32(5, 3) == 2)
    @assert(toto's32(-5, 3) == -2)
    @assert(toto's32(-5, -3) == -2)

    @assert(toto's64(6) == 0)
    @assert(toto's64(5) == 2)
    @assert(toto's64(6, 3) == 0)
    @assert(toto's64(5, 3) == 2)
    @assert(toto's64(-5, 3) == -2)
    @assert(toto's64(-5, -3) == -2)
}
