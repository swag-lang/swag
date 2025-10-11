struct ErrToto {}
func toto() throw
{
    throw ErrToto{}
}

struct AA { x, y = 1 }

//#[Swag.PrintBc]
func titi()->AA throw
{
    try toto()
    return {10, 20}
}

//#[Swag.PrintBc]
#test
{
    let v = catch titi()
    @assert(v.x == 1)
    @assert(v.y == 1)
}
