#global testerror "Err0211"

struct A {}
impl A
{
    mtd opPostCopy() {}
}
//#[Swag.PrintBc]
#test
{
    var a: A
    a.opPostCopy()
}
