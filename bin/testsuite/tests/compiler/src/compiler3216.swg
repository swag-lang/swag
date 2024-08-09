#global testerror "Err0574"
struct A {}
struct B {}

struct C {}
impl C
{
    #[Swag.Overload]
    mtd toto(x: A){}
    #[Swag.Overload]
    mtd toto(x: B){}
    #[Swag.Overload]
    mtd toto(x, y: s32){}
}

#test
{
    var c: C
    c.toto(1.0, 2.0)
}