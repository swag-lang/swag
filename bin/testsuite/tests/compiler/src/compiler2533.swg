struct ErrToto {}
func toto() throw
{
    throw ErrToto{}
}

func titi()->string throw
{
    try toto()
    return "666"
}

//#[Swag.PrintBc]
#test
{
    let x = catch titi()
    @assert(x == null)
}
