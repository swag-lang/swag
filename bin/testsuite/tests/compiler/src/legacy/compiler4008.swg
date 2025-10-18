#global testerror "Err0724"

struct A {}
impl A
{
    mtd toto() {}

    mtd const titi()
    {
        toto()
    }
}
