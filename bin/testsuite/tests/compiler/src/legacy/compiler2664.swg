struct AA
{
    v:   u32
}

impl AA
{
    func opAffect(me, x: u32)
    {
        //@print("affect\n")
        me.v = x
    }
}

func toto(y: s32, x: AA = 5)
{
    @assert(x.v == 5)
}

//#[Swag.PrintBc]
#test
{
    toto(10)
}
