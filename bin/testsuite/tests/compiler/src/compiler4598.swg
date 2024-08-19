#global testerror "Err0435"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}