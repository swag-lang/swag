#global testerror "Err0529"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}