#global testerror "Err0720"

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
