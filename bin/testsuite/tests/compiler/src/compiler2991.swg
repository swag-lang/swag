struct AA
{

}

interface BB
{
    toto: func(self, *u8)
}

impl AA
{
    func toto(using self, v: u64, a: s32 = 0)
    {
    }
}

impl BB for AA
{
    func impl toto(using self, ptr: *u8)
    {
        toto(0)
    }
}