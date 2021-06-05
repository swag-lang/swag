func toto() throw
{
    throw "toto"
}

//#[Swag.printbc]
func titi()->[2] s32 throw
{
    try toto()
    return @[10, 20]
}

//#[Swag.printbc]
#test
{
    v := catch titi()
    @assert(v[0] == 0)
    @assert(v[1] == 0)
}
