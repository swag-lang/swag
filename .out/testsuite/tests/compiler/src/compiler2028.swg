struct AA
{
    x, y, z: f64 = 666
}

//#[Swag.PrintBc]
func inc(x: AA)->AA
{
    return x
}

#test
{
    var v: AA
    v1 := inc(v)
    @assert(v1.x == 666)
    v1.x = 555
    @assert(v.x == 666)
    @assert(v1.x == 555)
}