#global testerror "Err0494"
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    var x = "titi"
    toto(x)
}