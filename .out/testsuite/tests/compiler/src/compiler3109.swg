interface Itf
{
    toto: func(self)
}

struct AA
{
    v: s32
}

impl Itf for AA
{
    mtd toto()
    {
        mtd YYY() => 666
        @print(YYY(self))
    }
}