interface Itf
{
    func toto(self);
}

struct AA
{
    v: s32
}

impl Itf for AA
{
    mtd YYY()->s32
    {
        return 666
    }

    mtd impl toto()
    {
        @print(YYY(self))
    }
}
