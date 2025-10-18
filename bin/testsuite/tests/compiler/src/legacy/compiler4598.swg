#global testerror "Err0609"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}
