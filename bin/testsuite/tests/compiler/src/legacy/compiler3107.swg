interface Itf
{
    func toto(me);
}

struct AA
{
    v:   s32
}

impl AA
{
    mtd YYY()->s32
    {
        return 666
    }
}

impl Itf for AA
{
    mtd impl toto()
    {
        @print(YYY(me))
    }
}
