#global testerror "Err0712"

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
