#global testerror "Err0434"
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