struct C
{
    vv:  s32
}

impl C
{
    #[Swag.ConstExpr]
    mtd opAffect(x: s32)
    {
        me.vv = x
    }
}

func toto(x: C)
{
    @assert(x.vv == 4)
}

#test
{
    const c: C = 4
    toto(c)
}
