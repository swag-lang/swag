interface I
{
    mtd toto();
}

struct S {}
impl I for S
{
    mtd impl toto() {}
}
