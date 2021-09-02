using Swag

struct AA
{
    x, y = 555
}

#[Inline]
func toto()->[2] AA throw
{
    throw "coucou"
    return @[{1, 2}, {1, 2}]
}

//#[PrintBc]
#test
{
    x := catch toto()
    @assert(x[0].x == 555)
    @assert(x[1].y == 555)
}