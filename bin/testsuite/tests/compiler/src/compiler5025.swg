#global testerror "Err0107"

struct A{}
impl A
{
    mtd opPostCopy(){}
}
//#[Swag.PrintBc]
#test
{
    var a: A
    a.opPostCopy()
}