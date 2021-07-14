using Swag

#[inline]
func toto()->string throw
{
    throw "coucou"
    return "10"
}

//#[PrintBc]
#test
{
    x := catch toto()
    @assert(x == null)
}