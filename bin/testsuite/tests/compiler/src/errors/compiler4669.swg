interface I
{
    mtd toto();
}

struct A {}
impl I for A
{
    mtd impl toto() {}
}

//#global testerror
//#[Swag.PrintBc]
#test
{
    const a  = A{}
    const x: I = cast(I) a
    var y:   I
    const z  = x orelse y
}
