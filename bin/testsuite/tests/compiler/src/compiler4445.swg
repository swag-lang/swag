#global testerror "Err0280"
interface I
{
    mtd toto()->s32
}

struct A{}
impl I for A
{
    mtd toto()
    {

    }
}