func toto()
{
    @seterr("toto")
}

func titi()->s64
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
