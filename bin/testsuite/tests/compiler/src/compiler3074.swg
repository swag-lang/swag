struct AA { x, y: f32 }

func toto(a, b: AA)->AA
{
    var result: retval
    result.x = a.x + b.x
    result.y = a.y + b.y
    return result
}

//#[Swag.PrintBc]
#[Swag.Optimize(.All, true)]
#test
{
    var x = AA{1, 1}
    var y = AA{2, 2}
    x = toto(x, y)
    @assert(x.x == 3.0)
    @assert(x.y == 3.0)
}
