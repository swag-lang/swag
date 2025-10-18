#global testerror "Err0407"

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
