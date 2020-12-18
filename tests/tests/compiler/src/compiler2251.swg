struct A
{
    x: s32
}

func titi()->A
{
    var result: retval
    result.x = 666
    return result
}

var G: A

//#[swag.printbc]
#test
{
    G = titi()
    @assert(G.x == 666)
}