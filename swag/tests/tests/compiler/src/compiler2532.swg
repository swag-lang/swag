func toto()
{
    @seterr("toto")
}

func titi()->s64
{
    toto() try
    return 666
}

//#[swag.printbc]
#test
{
    x := titi() catch {}
    @assert(x == 0)
}
