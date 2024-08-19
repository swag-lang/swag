#global testerror "Err0090"

struct A{}
impl A
{
    mtd opPostMove(){}
}
//#[Swag.PrintBc]
#test
{
    var a: A
    a.opPostMove()
}