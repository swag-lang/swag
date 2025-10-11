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
    mtd YYY()->s32
    {
        return 666
    }

    mtd impl toto()
    {
        @print(YYY(me))
    }
}
