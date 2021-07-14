var G: bool

func toto(x: s64)
{
    @assert(x == 666)
    G = true
}

//#[Swag.PrintBc]
#test
{
    G = false
    var x: func(s64) = &toto
    tt := 666's64
    tt.x()
    @assert(G == true)
}