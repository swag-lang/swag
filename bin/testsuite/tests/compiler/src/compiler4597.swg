#global testerror "Err0723"

interface I
{
    mtd toto()
}

struct S {}

impl I for S
{
    mtd impl titi() {}
}