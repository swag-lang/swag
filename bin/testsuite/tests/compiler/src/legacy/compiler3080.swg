interface AA
{
    func toto(me);
}

struct BB
{
    x:   s32 = 666
}

var G: s32
impl AA for BB
{
    func titi(me)
    {
        G = me.x
    }

    func impl toto(me)
    {
        me.titi()
    }
}

#test
{
    var v: BB
    let i  = cast(AA) v
    i.toto()
    @assert(G == 666)
}
