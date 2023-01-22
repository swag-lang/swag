using Swag

func toto(a: s32, #[ConstExpr] str: string)
{
}

#test
{
    toto(0, "titi")
}