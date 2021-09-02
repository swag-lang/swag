struct AA
{
    x, y, z: f64 = 666
}

func inc(x: AA)->AA
{
    var y: AA = x
    return y
}

#test
{
    var v: AA
    @assert(inc(v).x == 666)
}