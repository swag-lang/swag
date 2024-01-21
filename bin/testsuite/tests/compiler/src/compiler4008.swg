#global testerror "Err0541"

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
