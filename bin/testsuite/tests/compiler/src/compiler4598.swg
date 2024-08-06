#global testerror "Err0485"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}