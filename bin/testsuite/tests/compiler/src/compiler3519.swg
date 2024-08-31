#global testerror "Err0546"
struct A {}
impl A
{
    #[Swag.Overload]
    mtd toto() {}
    #[Swag.Overload]
    mtd toto(x: s32) {}
}

func titi(x: A)
{
    x.toto()
}