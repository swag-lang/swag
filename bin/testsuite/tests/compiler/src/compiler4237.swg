#global testerror "Err0438"
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    var x = "titi"
    toto(x)
}