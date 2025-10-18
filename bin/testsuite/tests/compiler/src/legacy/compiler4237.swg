#global testerror "Err0290"
using Swag

func toto(#[ConstExpr] str: string) {}

#test
{
    var x = "titi"
    toto(x)
}
