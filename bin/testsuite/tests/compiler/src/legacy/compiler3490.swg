#global testerror "Err0090"

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
