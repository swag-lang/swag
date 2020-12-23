struct AA
{
    x: s32
}

impl AA
{
    func toto(using self, x: s32)
    {
        self.x = #scopefct.x
    }
}