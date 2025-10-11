#global testerror "Err0434"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}
