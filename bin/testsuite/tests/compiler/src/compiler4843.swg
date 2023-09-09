#global testerror "Err0095"
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