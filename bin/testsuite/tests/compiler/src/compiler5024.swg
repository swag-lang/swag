#global testerror "Err0108"

struct A{}
impl A
{
    mtd opDrop(){}
}
//#[Swag.PrintBc]
#test
{
    var a: A
    a.opDrop()
}