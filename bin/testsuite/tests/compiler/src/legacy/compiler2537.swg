struct AA
{
    x    = 0
}

struct ErrToto {}

//#[Swag.PrintBc]
func toto()->AA throw
{
    throw ErrToto{}
    return {1}
}

//#[Swag.PrintBc]
func titi()->AA throw
{
    return try toto()
}

#test
{
    var x = catch titi()
    @assert(@err == ErrToto)
}
