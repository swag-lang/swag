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
    method toto()
    {
        method YYY() => 666
        @print(YYY(self))
    }
}