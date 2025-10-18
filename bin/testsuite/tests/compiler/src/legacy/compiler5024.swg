#global testerror "Err0210"

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
