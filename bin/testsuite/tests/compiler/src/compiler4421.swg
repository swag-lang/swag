var G = 0

func toto(x: &&s32)
{
    @assert(false)
}

func toto(x: &s32)
{
    G = 1
}

#test
{
    var x = 0
    toto(&x)
    @assert(G == 1)
}