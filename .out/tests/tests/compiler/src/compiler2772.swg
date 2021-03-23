func(T) toto()
{
    x := cast(T) 2
    y := cast(T) 1
    @assert(@min(x, y) == 1)
    @assert(@max(x, y) == 2)
}

#test
{
    toto's8()
    toto's16()
    toto's32()
    toto's64()
}