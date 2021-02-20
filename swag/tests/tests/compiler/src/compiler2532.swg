func toto() throw
{
    throw "toto"
}

func titi()->s64 throw
{
    try toto()
    return 666
}

//#[swag.printbc]
#test
{
    x := catch titi()
    @assert(x == 0)
}
