interface I
{
    mtd toto();
}
struct A {}
impl I for A
{
    mtd impl toto() {}
}

//#global skip
//#[Swag.PrintBc]
#test
{
    const a: I = null
    @assert(a == null)
}
