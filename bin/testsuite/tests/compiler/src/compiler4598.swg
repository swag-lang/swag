#global testerror "Err0420"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}