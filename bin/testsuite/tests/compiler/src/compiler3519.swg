#global testerror "Err0113"
struct A {}
impl A
{
    mtd toto() {}
    mtd toto(x: s32) {}
}

func titi(x: A)
{
    x.toto()
}