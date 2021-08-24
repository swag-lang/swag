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
    method YYY()->s32
    {
        return 666
    }

    method toto()
    {
        @print(YYY(self))
    }
}