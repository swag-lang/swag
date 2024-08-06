#global testerror "Err0546"

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
