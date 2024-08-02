#global testerror "Err0713"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}