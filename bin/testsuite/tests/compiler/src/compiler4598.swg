#global testerror "Err0461"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}