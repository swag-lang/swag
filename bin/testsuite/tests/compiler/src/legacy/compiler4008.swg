#global testerror "Err0487"

struct A {}
impl A
{
    mtd toto() {}

    mtd const titi()
    {
        toto()
    }
}
