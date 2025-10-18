#global testerror "Err0092"
interface I
{
    mtd toto();
}

struct S {}

impl I for S
{
    mtd impl tota() {}
}
