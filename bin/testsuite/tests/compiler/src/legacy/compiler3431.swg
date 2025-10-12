struct X
{
    v:   func||(s32)
}

var G = 0
func toto(x: s32)
{
    G = x
    @assert(x == 5)
}

//#[Swag.PrintBc, Swag.Optimize(.All, false)]
#test
{
    var x: X
    x.v = &toto
    x.v(5)
    @assert(G == 5)
}
