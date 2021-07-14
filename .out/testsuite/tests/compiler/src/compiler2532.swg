//#[Swag.PrintBc]
func toto() throw
{
    throw "toto"
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
    x := catch titi()
    @assert(x == 0)
}
