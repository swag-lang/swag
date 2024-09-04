#global testerror "Err0423"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}