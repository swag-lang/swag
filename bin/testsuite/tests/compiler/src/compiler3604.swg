//#global skip
struct A {}

interface I
{
    func toto(me);
}

impl I for A
{
    enum XX
    {
        R
    }

    struct XX1 {}

    alias XX2 = s32

    mtd impl toto()
    {
        var x:  XX = undefined
        var x1: XX1 = undefined
        var x2: XX2 = undefined
    }
}
