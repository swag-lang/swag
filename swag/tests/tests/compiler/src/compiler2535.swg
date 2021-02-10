func toto()
{
    @seterr("toto")
}

//#[swag.printbc]
func titi()->[2] s32
{
    toto() try
    return @[10, 20]
}

//#[swag.printbc]
#test
{
    v := titi() catch {}
    @assert(v[0] == 0)
    @assert(v[1] == 0)
}
