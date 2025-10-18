#global testerror "Err0539"
struct A {}
impl A
{
    #[Swag.ConstExpr]
    mtd opAffect(x: bool) {}

    mtd toto() {}
}

#test
{
    const a: A = true
    a.toto()
}
