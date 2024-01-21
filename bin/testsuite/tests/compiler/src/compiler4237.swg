#global testerror "Err0493"
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    var x = "titi"
    toto(x)
}