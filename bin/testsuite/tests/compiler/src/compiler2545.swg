using Swag
struct ErrToto {}

#[Inline]
func toto()->[2] s32 throw
{
    throw ErrToto{}
    return [1, 2]
}

//#[PrintBc]
#test
{
    var x = catch toto()
    @assert(x[0] == 0)
}
