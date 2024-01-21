#global testerror "Err0718"

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
