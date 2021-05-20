//#[Swag.printbc]
func toto() throw
{
    throw "toto"
}

//#[Swag.printbc]
func titi()->s64 throw
{
    try toto()
    return 666
}

//#[Swag.printbc]
#test
{
    x := catch titi()
    @assert(x == 0)
}
