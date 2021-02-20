func toto() throw
{
    throw "toto"
}

struct AA
{
    x, y = 1
}

//#[swag.printbc]
func titi()->AA throw
{
    try toto()
    return @{10, 20}
}

//#[swag.printbc]
#test
{
    v := catch titi()
    @assert(v.x == 1)
    @assert(v.y == 1)
}
