using Swag
struct ErrToto {}

struct AA { x, y = 555 }

#[Inline]
func toto()->AA throw
{
    throw ErrToto{}
    return {1, 2}
}

//#[PrintBc]
#test
{
    let x = catch toto()
    @assert(x.x == 555)
}
