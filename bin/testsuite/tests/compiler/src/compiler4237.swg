#global testerror "Err0483"
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    var x = "titi"
    toto(x)
}