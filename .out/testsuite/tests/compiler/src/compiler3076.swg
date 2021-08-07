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

#[Swag.Tls]
var x, y: AA

#[Swag.PrintBc]
#[Swag.Optim("", true)]
#test
{
    x = AA{1, 2}
    y = AA{3, 4}
    x = toto(x, y)
    @assert(x.x == 4.0)
    @assert(x.y == 6.0)
}