#global testerror

interface I
{
    mtd toto()
}

struct S {}

impl I for S
{
    mtd toto() {}
}