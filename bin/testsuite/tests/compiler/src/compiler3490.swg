#global testerror "Err0694"

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
