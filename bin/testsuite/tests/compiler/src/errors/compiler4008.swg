#global testerror "Err0556"

struct A {}
impl A
{
    mtd toto() {}

    mtd const titi()
    {
        toto()
    }
}
