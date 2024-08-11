#global testerror "Err0420"
interface I
{
    mtd toto()->bool;
}

struct A{}
impl I for A
{
    mtd toto()->s32
    {
        return 0
    }
}