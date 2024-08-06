#global testerror "Err0705"

namespace A
{
}

struct B
{
}

impl B
{
    mtd titi()
    {
    }

    mtd toto()
    {
        A.titi()
    }
}
