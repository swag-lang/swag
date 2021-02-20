using swag

#[inline]
func toto()->[2] s32 throw
{
    throw "coucou"
    return @[1, 2]
}

//#[printbc]
#test
{
    x := catch toto()
    @assert(x[0] == 0)
}