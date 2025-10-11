#global testerror "Err0378"
using Swag

func toto(#[ConstExpr] str: string) {}

#test
{
    var x = "titi"
    toto(x)
}
