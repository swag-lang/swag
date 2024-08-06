#global testerror "Err0484"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}