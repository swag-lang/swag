//#global skip
struct A
{
}

interface I
{
    toto: func(self)
}

impl I for A
{
    enum XX
    {
        R
    }

    struct XX1
    {
    }

    alias XX2 = s32

    mtd toto()
    {
        var x: XX = ?
        var x1: XX1 = ?
        var x2: XX2 = ?
    }
}