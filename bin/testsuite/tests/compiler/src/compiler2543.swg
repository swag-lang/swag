using Swag

#[Inline]
func toto()->string throw
{
    throw "coucou"
    return "10"
}

//#[PrintBc]
#test
{
    var x = catch toto()
    @assert(x == null)
}