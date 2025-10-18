#global testerror "Err0371"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}
