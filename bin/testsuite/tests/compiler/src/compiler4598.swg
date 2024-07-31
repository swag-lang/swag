#global testerror "Err0539"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}