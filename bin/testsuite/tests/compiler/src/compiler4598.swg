#global testerror "Err0425"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}