struct ErrToto {}
func toto() throw
{
    throw ErrToto{}
}

//#[Swag.PrintBc]
func titi()->[2] s32 throw
{
    try toto()
    return [10, 20]
}

//#[Swag.PrintBc]
#test
{
    var v = catch titi()
    @assert(v[0] == 0)
    @assert(v[1] == 0)
}
