#global testerror "Err1165"
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
}