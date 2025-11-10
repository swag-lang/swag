struct AA {}

interface BB
{
    func toto(me, x: *u8);
}

impl AA
{
    func toto(me, v: u64, a: s32 = 0) {}
}

impl BB for AA
{
    func impl toto(me, ptr: *u8)
    {
        me.toto(0)
    }
}
