struct AA { x, y, z: f64 = 666 }

//#[Swag.PrintBc]
func inc(x: AA)->AA
{
    let y = x
    return y
}

#test
{
    var v: AA
    var v1 = inc(v)
    @assert(v1.x == 666)
    v1.x = 555
    @assert(v.x == 666)
    @assert(v1.x == 555)
}
