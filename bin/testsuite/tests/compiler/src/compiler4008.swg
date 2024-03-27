#global testerror "Err0540"

struct A
{
}
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
