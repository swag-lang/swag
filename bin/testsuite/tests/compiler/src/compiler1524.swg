var G: s32

struct X
{
    x: s32 = 666
    y: bool   
}

impl X
{
    func toto(using const self)
    {
        G = x
    }
}

func titi(str: X)
{
    str.toto()
}

#test
{
    G = 0
    var p: X
    titi(p)
    @assert(G == 666)
}
