#global testerror "Err0539"
interface I
{
    mtd toto();
}

struct A{}
impl I for A
{
    mtd toto()->s32
    {
        return 0
    }
}