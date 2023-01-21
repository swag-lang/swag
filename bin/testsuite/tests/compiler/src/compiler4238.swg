using Swag

func toto(#[ConstExpr] str: string)
{
}

#test
{
    toto("titi")
}