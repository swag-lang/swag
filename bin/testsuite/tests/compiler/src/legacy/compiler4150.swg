#global testerror "Err0734"
struct A {}

impl A
{
    mtd toto(x, y: s32) {}
    {
    }

    mtd titi()
    {
        me.toto(1, 2, false)
    }
}
