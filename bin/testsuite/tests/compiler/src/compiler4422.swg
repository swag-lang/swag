var G = 0

func toto(x: moveref s32)
{
    G = 1
}

func toto(x: ref s32)
{
    @assert(false)
}

#test
{
    var x = 0
    toto(moveref &x)
    @assert(G == 1)
}