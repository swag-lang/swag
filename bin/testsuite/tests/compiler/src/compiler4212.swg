func(T) toto(y: T)->T
{
    x := y
    x /= 3
    return x
}

func(T) toto(y, z: T)->T
{
    x1 := y
    x2 := z
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
}