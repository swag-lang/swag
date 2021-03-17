func toto() throw
{
    throw "toto"
}

struct AA
{
    x, y = 1
}

//#[swag.printbc]
func titi()->[3] AA throw
{
    try toto()
    return @[{10, 20}, {10, 20}, {10, 20}]
}

//#[swag.printbc]
#test
{
    v := catch titi()
    @assert(v[0].x == 1)
    @assert(v[2].y == 1)
}
