interface AA
{
    func toto(me);
}

struct BB
{
    x: s32 = 666
}

var G: s32
impl AA for BB
{
    func titi(using me)
    {
        G = x
    }

    func impl toto(using me)
    {
        titi()
    }
}

#test
{
    var v: BB
    let i = cast(AA) v
    i.toto()
    @assert(G == 666)
}