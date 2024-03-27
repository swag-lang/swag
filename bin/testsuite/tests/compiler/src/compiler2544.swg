using Swag
struct ErrToto{}

struct AA
{
    x, y = 555
}

#[Inline]
func toto()->AA throw
{
    throw ErrToto{}
    return {1, 2}
}

//#[PrintBc]
#test
{
    var x = catch toto()
    @assert(x.x == 555)
}