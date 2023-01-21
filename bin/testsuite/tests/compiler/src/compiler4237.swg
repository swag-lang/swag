#global testerror
using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    x := "titi"
    toto(x)
}