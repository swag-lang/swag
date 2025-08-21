struct AA
{
    x: s32
}

impl AA
{
    func toto(using me, x: s32)
    {
        me.x = #me.x
    }
}