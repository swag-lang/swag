func toto()
{
    @seterr("toto")
}

struct AA
{
    x, y = 1
}

#[swag.printbc]
func titi()->AA
{
    toto() try
    return @{10, 20}
}

//#[swag.printbc]
#test
{
    v := titi() catch {}
    @assert(v.x == 1)
    @assert(v.y == 1)
}
