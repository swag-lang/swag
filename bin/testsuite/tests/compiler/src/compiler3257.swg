#global testerror "Err0004"

struct AA {}
impl AA
{
    mtd toto(x: s32 = 0)
    {
        var v: AA
        using v
        toto()
    }
}