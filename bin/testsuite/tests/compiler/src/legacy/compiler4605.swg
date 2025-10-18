#global testerror "Err0090"
interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl tota() {}
}
