struct ErrToto {}
//#[Swag.PrintBc]
func toto() throw
{
    throw ErrToto{}
}

//#[Swag.PrintBc]
func titi()->s64 throw
{
    try toto()
    return 666
}

//#[Swag.PrintBc]
#test
{
    let x = catch titi()
    @assert(x == 0)
}
