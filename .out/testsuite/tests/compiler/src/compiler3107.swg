interface Itf
{
    toto: func(self)
}

struct AA
{
    v: s32
}

impl AA
{
    method YYY()->s32
    {
        return 666
    }
}

impl Itf for AA
{
    method toto()
    {
        @print(YYY(self))
    }
}