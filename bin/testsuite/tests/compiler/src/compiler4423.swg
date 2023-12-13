var G = 0

func toto(x: moveref s32)
    #validif true
{
    G = 1
}

func toto(x: &s32)
    #validif true
{
    @assert(false)
}

#test
{
    var x = 0
    toto(moveref &x)
    @assert(G == 1)
}