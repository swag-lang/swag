interface Itf
{
    func toto(me);
}

struct AA
{
    v:   s32
}

impl Itf for AA
{
    mtd impl toto()
    {
        mtd YYY() => 666
        @print(YYY(me))
    }
}
