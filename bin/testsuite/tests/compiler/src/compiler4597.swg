#global testerror "Err0710"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}