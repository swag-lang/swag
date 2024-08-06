#global testerror "Err0094"

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