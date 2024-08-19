#global testerror "Err0438"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}