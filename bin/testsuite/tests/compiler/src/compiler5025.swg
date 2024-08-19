#global testerror "Err0089"

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