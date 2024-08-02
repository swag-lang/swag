#global testerror "Err0523"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}