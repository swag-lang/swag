func toto()
{
    throw "toto"
}

//#[swag.printbc]
func titi()->[2] s32
{
    try toto()
    return @[10, 20]
}

//#[swag.printbc]
#test
{
    v := catch titi()
    @assert(v[0] == 0)
    @assert(v[1] == 0)
}
