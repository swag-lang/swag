#global testerror "Err0604"
struct A {}
impl A
{
    mtd toto(x: s32) {}
    mtd titi()
    {
        toto(0)
    }
}
