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
    mtd impl toto()
    {
        mtd YYY() => 666
        @print(YYY(self))
    }
}