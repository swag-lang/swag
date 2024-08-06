#global testerror "Err0095"

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