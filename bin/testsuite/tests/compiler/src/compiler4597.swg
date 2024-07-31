#global testerror "Err0728"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}