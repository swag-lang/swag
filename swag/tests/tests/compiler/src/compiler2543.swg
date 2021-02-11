using swag

#[inline]
func toto()->string
{
    throw "coucou"
    return "10"
}

#[printbc]
#test
{
    x := catch toto()
    @assert(x == null)
}