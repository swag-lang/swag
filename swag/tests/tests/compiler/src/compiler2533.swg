func toto()
{
    @seterr("toto")
}

func titi()->string
{
    toto() try
    return "666"
}

//#[swag.printbc]
#test
{
    x := titi() catch {}
    @assert(x == null)
}
