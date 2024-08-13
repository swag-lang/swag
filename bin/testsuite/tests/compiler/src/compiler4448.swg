#global testerror "Err0427"
interface I
{
    mtd toto()->bool throw;
}

struct A{}
impl I for A
{
    mtd toto()->bool
    {
        return true
    }
}