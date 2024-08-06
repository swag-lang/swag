#global testerror "Err0523"

struct A {}
impl A
{
    mtd toto()
    {
    }

    mtd const titi()
    {
        toto()
    }
}
