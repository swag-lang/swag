#global testerror "Err0406"

struct A {}
impl A
{
    mtd opDrop() {}
}
//#[Swag.PrintBc]
#test
{
    var a: A
    a.opDrop()
}
