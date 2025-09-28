#global testerror "Err0088"

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
