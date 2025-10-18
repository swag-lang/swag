#global testerror "Err0091"

namespace A
{
}

struct B {}

impl B
{
    mtd titi() {}

    mtd toto()
    {
        A.titi()
    }
}
