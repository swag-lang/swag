struct(T) Point
{
    x, y: T
}

func(T) RR()->{Point, err: bool}
{
    return @{{1, 2}, true}
}

#test
{
    res := RR'f32()
    @assert(@typeof(res.item0.x) == f32)
    @assert(res.item0.x == 1)
    @assert(res.item0.y == 2)
    @assert(res.item1 == true)
    @assert(res.err == true)
}
