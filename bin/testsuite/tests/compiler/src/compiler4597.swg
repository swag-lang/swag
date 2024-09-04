#global testerror "Err0674"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}