#global testerror "Err0472"
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    var x = "titi"
    toto(x)
}