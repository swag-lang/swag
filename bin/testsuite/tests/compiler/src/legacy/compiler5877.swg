#global testerror "Err0745"
struct A {}
impl A
{
    mtd toto(x: s32) {}
    mtd titi()
    {
        toto(0)
    }
}
