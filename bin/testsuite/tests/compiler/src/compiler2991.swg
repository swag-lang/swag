struct AA
{

}

interface BB
{
    toto: func(self, *u8)
}

impl AA
{
    func toto(using self, v: uint, a: s32 = 0)
    {
    }
}

impl BB for AA
{
    func toto(using self, ptr: *u8)
    {
        toto(0)
    }
}