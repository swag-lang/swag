#global testerror "Err0579"
struct A {}
impl A
{
    #[Swag.ConstExpr]
    mtd opAffect(x: bool) {}

    mtd toto() {}
}

namespace C
{
    const B: A = true
}

#test
{
    C.B.toto()
}   