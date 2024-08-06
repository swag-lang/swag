#global testerror "Err0159"
using Swag

struct A {}
impl A
{
    mtd toto()->A => {}
}

func titi(x: [..] A) {}

//#[Swag.PrintBc]
#test
{
    var a: [2] A
    titi(&a[..])
}
