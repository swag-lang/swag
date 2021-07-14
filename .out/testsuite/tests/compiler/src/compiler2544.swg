using Swag

struct AA
{
    x, y = 555
}

#[inline]
func toto()->AA throw
{
    throw "coucou"
    return @{1, 2}
}

//#[PrintBc]
#test
{
    x := catch toto()
    @assert(x.x == 555)
}