using Swag
struct ErrToto {}

#[Inline]
func toto()->string throw
{
    throw ErrToto{}
    return "10"
}

//#[PrintBc]
#test
{
    let x = catch toto()
    @assert(x == null)
}
