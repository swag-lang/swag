interface AA
{
    toto: func(self)
}

struct BB
{
    x: s32 = 666
}

var G: s32
impl AA for BB
{
    func titi(using self)
    {
        G = x
    }

    func impl toto(using self)
    {
        titi()
    }
}

#test
{
    var v: BB
    i := cast(AA) v
    i.toto()
    @assert(G == 666)
}