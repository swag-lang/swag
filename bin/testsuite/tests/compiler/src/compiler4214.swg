func(T) toto(y: T)->T
{
    x := y
    x %= 3
    return x
}

func(T) toto(y, z: T)->T
{
    x1 := y
    x2 := z
    x2 += 1
    x2 -= 1
    x1 %= x2
    return x1
}

#test
{
    @assert(toto's8(6) == 0)
    @assert(toto's8(5) == 2)
    @assert(toto's8(6, 3) == 0)
    @assert(toto's8(5, 3) == 2)
}