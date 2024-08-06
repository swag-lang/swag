#global testerror "Err0585"
struct A {}

impl A
{
    mtd toto(x, y: s32) {}
    {
    }

    mtd titi()
    {
        toto(1, 2, false)
    }
}