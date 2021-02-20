using swag

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

//#[printbc]
#test
{
    x := catch toto()
    @assert(x.x == 555)
}