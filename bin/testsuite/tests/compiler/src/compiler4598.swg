#global testerror "Err0483"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}