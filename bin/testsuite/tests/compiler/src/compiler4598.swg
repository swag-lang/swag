#global testerror "Err0280"

interface I
{
    mtd toto()
}

struct S {}

impl I for S
{
    mtd toto() {}
}