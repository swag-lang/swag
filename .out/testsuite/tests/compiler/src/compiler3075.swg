struct AA
{
    x, y: f32
}

func toto(a, b: AA)->AA
{
    var result: retval
    result.x = a.x + b.x
    result.y = a.y + b.y
    return result
}

var x, y: AA

//#[Swag.PrintBc]
#[Swag.Optim("", true)]
#test
{
    x = AA{1, 1}
    y = AA{2, 2}
    x = toto(x, y)
    @assert(x.x == 3.0)
    @assert(x.y == 3.0)
}