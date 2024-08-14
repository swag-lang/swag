#global testerror "Err0485"

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
