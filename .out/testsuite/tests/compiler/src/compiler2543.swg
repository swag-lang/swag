using Swag

#[inline]
func toto()->string throw
{
    throw "coucou"
    return "10"
}

//#[printbc]
#test
{
    x := catch toto()
    @assert(x == null)
}