#global testerror "Err0534"

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
