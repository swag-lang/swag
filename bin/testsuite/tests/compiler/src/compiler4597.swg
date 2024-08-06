#global testerror "Err0712"

interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}