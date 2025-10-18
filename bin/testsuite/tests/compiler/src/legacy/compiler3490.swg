#global testerror "Err0089"

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
