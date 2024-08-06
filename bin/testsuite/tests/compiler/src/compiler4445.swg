#global testerror "Err0485"
interface I
{
    mtd toto()->s32;
}

struct A{}
impl I for A
{
    mtd toto()
    {

    }
}