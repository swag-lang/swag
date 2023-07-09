func toto() throw
{
    throw "err"
}

//#[Swag.PrintBc]
func titi()->s32
{
    trycatch toto()
    @assert(false)
    return 666
}

#test
{
    var x = titi()
    @assert(x == 0)
}