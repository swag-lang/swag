#global testerror "Err0518"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd toto() {}
}