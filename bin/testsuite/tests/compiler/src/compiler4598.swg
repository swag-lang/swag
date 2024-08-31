#global testerror "Err0468"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}