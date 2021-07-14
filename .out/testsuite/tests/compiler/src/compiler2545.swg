using Swag

#[Inline]
func toto()->[2] s32 throw
{
    throw "coucou"
    return @[1, 2]
}

//#[PrintBc]
#test
{
    x := catch toto()
    @assert(x[0] == 0)
}