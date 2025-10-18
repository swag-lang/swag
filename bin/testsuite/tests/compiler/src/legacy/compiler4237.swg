#global testerror "Err0321"
using Swag

func toto(#[ConstExpr] str: string) {}

#test
{
    var x = "titi"
    toto(x)
}
